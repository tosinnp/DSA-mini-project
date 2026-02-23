#include "mainwindow.h"
#include <QApplication>
#include <QScrollBar>
#include <QString>
#include <QStringList>
#include <QColor>
#include <algorithm>

// ─────────────────────────────────────────────
//  COLOR PALETTE
// ─────────────────────────────────────────────
#define C_BG     "#0D1117"
#define C_CARD   "#161B22"
#define C_CARD2  "#1C2333"
#define C_BORDER "#30363D"
#define C_RED    "#FF4757"
#define C_PINK   "#FF6B9D"
#define C_CYAN   "#00D2FF"
#define C_GREEN  "#2ECC71"
#define C_ORANGE "#FF8C00"
#define C_YELLOW "#FFD700"
#define C_PURPLE "#BD93F9"
#define C_TEXT   "#E6EDF3"
#define C_SUBTLE "#8B949E"

// ─────────────────────────────────────────────
//  WIDGET HELPERS
// ─────────────────────────────────────────────
static QLabel* mkLabel(const QString& t, int sz,
                       const QString& col = C_TEXT, bool bold = false)
{
    auto* l = new QLabel(t);
    l->setStyleSheet(
        QString("color:%1;font-size:%2px;font-weight:%3;background:transparent;")
            .arg(col).arg(sz).arg(bold ? "700" : "400"));
    l->setWordWrap(true);
    return l;
}

static QPushButton* mkBtn(const QString& text,
                          const QString& bg,
                          const QString& fg    = "#ffffff",
                          const QString& hover = "")
{
    auto* b = new QPushButton(text);
    QString hov = hover.isEmpty() ? bg : hover;
    b->setStyleSheet(QString(R"(
        QPushButton {
            background:%1; color:%2;
            border:none; border-radius:10px;
            padding:9px 18px; font-size:13px;
            font-weight:600; letter-spacing:0.3px;
        }
        QPushButton:hover   { background:%3; }
        QPushButton:pressed { padding:10px 17px 8px 19px; }
    )").arg(bg, fg, hov));
    b->setCursor(Qt::PointingHandCursor);
    return b;
}

static QFrame* mkCard(QLayout* lay    = nullptr,
                      const QString& bg  = C_CARD,
                      const QString& brd = C_BORDER,
                      int radius = 16)
{
    auto* f = new QFrame();
    f->setStyleSheet(
        QString("QFrame{background:%1;border-radius:%2px;border:1.5px solid %3;}")
            .arg(bg).arg(radius).arg(brd));
    if (lay) { lay->setContentsMargins(16,14,16,14); f->setLayout(lay); }
    return f;
}

static QFrame* mkHLine() {
    auto* f = new QFrame();
    f->setFrameShape(QFrame::HLine);
    f->setStyleSheet(
        QString("color:%1;background:%1;border:none;max-height:1px;").arg(C_BORDER));
    return f;
}

// ─────────────────────────────────────────────
//  CONSTRUCTOR
// ─────────────────────────────────────────────
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowTitle("Nepal Election System — COMP 202 DSA · Kathmandu University");
    setMinimumSize(1150, 780);
    resize(1260, 860);

    qApp->setStyle("Fusion");
    QPalette pal;
    pal.setColor(QPalette::Window,     QColor(C_BG));
    pal.setColor(QPalette::WindowText, QColor(C_TEXT));
    pal.setColor(QPalette::Base,       QColor(C_CARD));
    pal.setColor(QPalette::Text,       QColor(C_TEXT));
    pal.setColor(QPalette::Button,     QColor(C_CARD));
    pal.setColor(QPalette::ButtonText, QColor(C_TEXT));
    qApp->setPalette(pal);

    setStyleSheet(QString(R"(
        QMainWindow { background:%1; }
        QScrollBar:vertical {
            background:%2; width:6px; border-radius:3px; }
        QScrollBar::handle:vertical {
            background:%3; border-radius:3px; min-height:20px; }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical { height:0; }
        QScrollBar:horizontal { height:0; }
    )").arg(C_BG, C_CARD, C_BORDER));

    // Register candidates (Linked List + BST)
    struct CD { int id; const char* name; const char* party; const char* color; };
    for (auto& d : std::vector<CD>{
                                   {101,"Ram Bahadur","Nepal Congress", C_CYAN},
                                   {102,"Sita Thapa", "CPN-UML",        C_GREEN},
                                   {103,"Hari Sharma","RPP Nepal",       C_ORANGE},
                                   {104,"Maya Gurung","Independent",     C_PURPLE},
                                   }) {
        candidates.add(d.id, d.name, d.party, d.color);
        candidateBST.insert(d.id);
    }

    buildUI();
    refreshAll();
    addLog("Election system initialized and ready.", C_GREEN);
    addLog("Select a candidate -> Enter Voter ID -> Join Queue -> Process.", C_CYAN);
}

MainWindow::~MainWindow() {}

// ─────────────────────────────────────────────
//  BUILD UI
// ─────────────────────────────────────────────
void MainWindow::buildUI()
{
    auto* central = new QWidget();
    setCentralWidget(central);
    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(22,18,22,18);
    root->setSpacing(14);

    // HEADER
    {
        auto* hRow = new QHBoxLayout();
        hRow->setSpacing(16);

        auto* titleCol = new QVBoxLayout();
        titleCol->setSpacing(5);

        // Nepal flag stripe
        auto* flagBar = new QFrame();
        flagBar->setFixedHeight(6);
        flagBar->setStyleSheet(
            "QFrame{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
            "stop:0 #E63946,stop:0.5 #E63946,stop:0.5 #1D3557,stop:1 #1D3557);"
            "border-radius:3px;border:none;}");
        titleCol->addWidget(flagBar);

        auto* badge = new QLabel("  LIVE ELECTION  |  KATHMANDU UNIVERSITY  |  COMP 202 DSA  ");
        badge->setStyleSheet(QString(
                                 "color:%1;background:rgba(230,57,70,0.15);"
                                 "border:1px solid rgba(230,57,70,0.4);"
                                 "border-radius:20px;font-size:11px;"
                                 "font-weight:600;letter-spacing:1.5px;padding:4px 12px;").arg(C_RED));
        badge->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        titleCol->addWidget(badge);

        auto* titleL = mkLabel("NEPAL ELECTION VOTE TALLYING SYSTEM", 24, C_TEXT, true);
        titleL->setStyleSheet(titleL->styleSheet() + "letter-spacing:1px;");
        titleCol->addWidget(titleL);

        titleCol->addWidget(mkLabel(
            "Linked List  |  Queue  |  Stack  |  BST  |  Hash Table  |  Selection Sort",
            11, C_SUBTLE));

        hRow->addLayout(titleCol, 1);

        // Stat cards
        auto mkStat = [&](const QString& label, const QString& color) -> QLabel* {
            auto* vl = new QVBoxLayout();
            vl->setSpacing(2);
            vl->setAlignment(Qt::AlignCenter);
            auto* num = mkLabel("0", 30, color, true);
            num->setAlignment(Qt::AlignCenter);
            auto* sub = mkLabel(label, 10, C_SUBTLE);
            sub->setAlignment(Qt::AlignCenter);
            sub->setStyleSheet(sub->styleSheet() + "letter-spacing:1.5px;");
            vl->addWidget(num);
            vl->addWidget(sub);
            auto* c = mkCard(vl, C_CARD2, color, 14);
            c->setFixedSize(120, 78);
            hRow->addWidget(c);
            return num;
        };

        statVotes    = mkStat("VOTES CAST", C_CYAN);
        statQueue    = mkStat("IN QUEUE",   C_ORANGE);
        statRejected = mkStat("REJECTED",   C_RED);

        root->addLayout(hRow);
    }

    root->addWidget(mkHLine());

    // BODY
    auto* body = new QHBoxLayout();
    body->setSpacing(16);

    // LEFT COLUMN
    auto* leftCol = new QVBoxLayout();
    leftCol->setSpacing(12);

    // Candidates scroll area
    {
        auto* secHdr = new QHBoxLayout();
        secHdr->addWidget(mkLabel("CANDIDATES", 12, C_SUBTLE));
        secHdr->addStretch();
        secHdr->addWidget(mkLabel("Click a card to select", 10, C_SUBTLE));
        leftCol->addLayout(secHdr);

        auto* scroll = new QScrollArea();
        scroll->setWidgetResizable(true);
        scroll->setStyleSheet("QScrollArea{border:none;background:transparent;}");
        auto* container = new QWidget();
        container->setStyleSheet("background:transparent;");
        candidatesLayout = new QVBoxLayout(container);
        candidatesLayout->setSpacing(10);
        candidatesLayout->setContentsMargins(0,0,0,0);
        scroll->setWidget(container);
        scroll->setMinimumHeight(300);
        leftCol->addWidget(scroll, 1);
    }

    // Vote panel
    {
        leftCol->addWidget(mkHLine());
        leftCol->addWidget(mkLabel("CAST YOUR VOTE", 12, C_SUBTLE));

        auto* pnl = new QVBoxLayout();
        pnl->setSpacing(10);

        voterInput = new QLineEdit();
        voterInput->setPlaceholderText("Enter Voter ID  (e.g.  V-001,  V-002 ...)");
        voterInput->setMinimumHeight(42);
        voterInput->setStyleSheet(QString(R"(
            QLineEdit {
                background:%1; color:%2;
                border:1.5px solid %3; border-radius:10px;
                padding:0 14px; font-size:13px;
            }
            QLineEdit:focus { border-color:%4; }
        )").arg(C_CARD2, C_TEXT, C_BORDER, C_CYAN));
        pnl->addWidget(voterInput);

        auto* btnRow = new QHBoxLayout();
        btnRow->setSpacing(8);
        auto* bJoin = mkBtn("Join Queue",    C_RED,   "#fff",    "#c0303c");
        auto* bNext = mkBtn("Process Next",  C_CARD2, C_CYAN,   "#252d3d");
        auto* bAll  = mkBtn("Process All",   C_CARD2, C_YELLOW,  "#252d3d");
        auto* bUndo = mkBtn("Undo",          C_CARD2, C_PINK,   "#252d3d");
        btnRow->addWidget(bJoin, 2);
        btnRow->addWidget(bNext, 2);
        btnRow->addWidget(bAll,  2);
        btnRow->addWidget(bUndo, 1);
        pnl->addLayout(btnRow);

        leftCol->addWidget(mkCard(pnl, C_CARD2, C_BORDER, 14));

        connect(bJoin, &QPushButton::clicked, this, &MainWindow::enqueueVoter);
        connect(bNext, &QPushButton::clicked, this, &MainWindow::processNext);
        connect(bAll,  &QPushButton::clicked, this, &MainWindow::processAll);
        connect(bUndo, &QPushButton::clicked, this, &MainWindow::undoLast);
        connect(voterInput, &QLineEdit::returnPressed, this, &MainWindow::enqueueVoter);
    }

    // Queue display
    {
        leftCol->addWidget(mkLabel("VOTER QUEUE  (FIFO)", 12, C_SUBTLE));
        queueList = new QListWidget();
        queueList->setFixedHeight(120);
        queueList->setStyleSheet(QString(R"(
            QListWidget {
                background:%1; border:1.5px solid %2;
                border-radius:12px; padding:4px; font-size:12px;
            }
            QListWidget::item {
                background:rgba(255,140,0,0.13); color:%3;
                border-radius:7px; padding:5px 10px; margin:2px 0;
            }
            QListWidget::item:selected { background:rgba(255,140,0,0.28); }
        )").arg(C_CARD2, C_BORDER, C_ORANGE));
        leftCol->addWidget(queueList);
    }

    body->addLayout(leftCol, 3);

    // RIGHT COLUMN
    auto* rightCol = new QVBoxLayout();
    rightCol->setSpacing(12);

    // Leaderboard
    {
        rightCol->addWidget(mkLabel("LIVE LEADERBOARD", 12, C_SUBTLE));
        auto* scroll = new QScrollArea();
        scroll->setWidgetResizable(true);
        scroll->setStyleSheet("QScrollArea{border:none;background:transparent;}");
        auto* container = new QWidget();
        container->setStyleSheet("background:transparent;");
        leaderboardLayout = new QVBoxLayout(container);
        leaderboardLayout->setSpacing(8);
        leaderboardLayout->setContentsMargins(0,0,0,0);
        scroll->setWidget(container);
        scroll->setMinimumHeight(280);
        rightCol->addWidget(scroll, 1);
    }

    // Log
    {
        rightCol->addWidget(mkHLine());
        rightCol->addWidget(mkLabel("ACTIVITY LOG", 12, C_SUBTLE));
        logList = new QListWidget();
        logList->setStyleSheet(QString(R"(
            QListWidget {
                background:%1; border:1.5px solid %2;
                border-radius:12px; font-size:12px; padding:4px;
            }
            QListWidget::item {
                border-radius:7px; padding:6px 10px; margin:2px 0;
            }
        )").arg(C_CARD2, C_BORDER));
        logList->setMinimumHeight(220);
        rightCol->addWidget(logList, 1);
    }

    body->addLayout(rightCol, 2);
    root->addLayout(body, 1);
}

// ─────────────────────────────────────────────
//  CANDIDATE CARDS
// ─────────────────────────────────────────────
void MainWindow::buildCandidateCards()
{
    QLayoutItem* item;
    while ((item = candidatesLayout->takeAt(0))) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int total = 0;
    Candidate* c = candidates.head;
    while (c) { total += c->votes; c = c->next; }

    c = candidates.head;
    while (c) {
        bool    sel = (c->id == selectedCandidateId);
        int     pct = total > 0 ? (c->votes * 100 / total) : 0;
        QString col = QString::fromStdString(c->color);
        QString bgStr  = sel
                            ? QString("rgba(%1,%2,%3,0.09)")
                                  .arg(QColor(col).red())
                                  .arg(QColor(col).green())
                                  .arg(QColor(col).blue())
                            : QString(C_CARD);
        QString brdStr = sel ? col : QString(C_BORDER);

        auto* row = new QHBoxLayout();
        row->setSpacing(14);
        row->setContentsMargins(16,14,16,14);

        // Avatar
        auto* av = new QLabel(QString(QChar(c->name[0])));
        av->setFixedSize(50,50);
        av->setAlignment(Qt::AlignCenter);
        av->setStyleSheet(QString(
                              "background:%1;color:%2;border-radius:25px;"
                              "font-size:20px;font-weight:700;").arg(col, C_BG));
        row->addWidget(av);

        // Info col
        auto* info = new QVBoxLayout();
        info->setSpacing(3);
        info->addWidget(mkLabel(QString::fromStdString(c->name), 14, C_TEXT, true));
        info->addWidget(mkLabel(
            QString::fromStdString(c->party) + "   |   ID: " + QString::number(c->id),
            11, C_SUBTLE));
        auto* bar = new QProgressBar();
        bar->setRange(0,100); bar->setValue(pct);
        bar->setTextVisible(false); bar->setFixedHeight(5);
        bar->setStyleSheet(QString(
                               "QProgressBar{background:#1F2937;border-radius:2px;border:none;}"
                               "QProgressBar::chunk{background:%1;border-radius:2px;}").arg(col));
        info->addWidget(bar);
        row->addLayout(info, 1);

        // Vote count
        auto* vc = new QVBoxLayout();
        vc->setAlignment(Qt::AlignCenter);
        auto* vNum = mkLabel(QString::number(c->votes), 26, col, true);
        vNum->setAlignment(Qt::AlignRight);
        auto* vPct = mkLabel(QString::number(pct)+"%", 11, C_SUBTLE);
        vPct->setAlignment(Qt::AlignRight);
        vc->addWidget(vNum); vc->addWidget(vPct);
        row->addLayout(vc);

        // Select button
        int cid = c->id;
        auto* selBtn = mkBtn(sel ? "Selected" : "Select",
                             sel ? col : QString(C_CARD2),
                             sel ? QString(C_BG) : QString(C_TEXT));
        selBtn->setFixedWidth(90);
        connect(selBtn, &QPushButton::clicked, this, [this, cid](){
            selectedCandidateId = cid;
            buildCandidateCards();
        });
        row->addWidget(selBtn);

        auto* frame = new QFrame();
        frame->setLayout(row);
        frame->setStyleSheet(QString(
                                 "QFrame{background:%1;border-radius:14px;border:1.5px solid %2;}")
                                 .arg(bgStr, brdStr));
        candidatesLayout->addWidget(frame);
        c = c->next;
    }
    candidatesLayout->addStretch();
}

// ─────────────────────────────────────────────
//  LEADERBOARD
// ─────────────────────────────────────────────
void MainWindow::buildLeaderboard()
{
    QLayoutItem* item;
    while ((item = leaderboardLayout->takeAt(0))) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    auto sorted = candidates.toVector();
    selectionSort(sorted);

    const QStringList medals     = {"1st","2nd","3rd"};
    const QStringList rankColors = {C_YELLOW,"#C0C0C0","#CD7F32"};

    int total = 0;
    Candidate* cur = candidates.head;
    while (cur) { total += cur->votes; cur = cur->next; }

    for (int i = 0; i < (int)sorted.size(); i++) {
        auto*   c      = sorted[i];
        QString col    = QString::fromStdString(c->color);
        bool    winner = (i == 0 && c->votes > 0);

        auto* row = new QHBoxLayout();
        row->setSpacing(14);
        row->setContentsMargins(16,12,16,12);

        // Rank label
        QString medal = i < 3 ? medals[i] : QString::number(i+1)+".";
        auto* rankL = mkLabel(medal, 14, i<3 ? rankColors[i] : QString(C_SUBTLE), true);
        rankL->setFixedWidth(40);
        rankL->setAlignment(Qt::AlignCenter);
        row->addWidget(rankL);

        // Name + party
        auto* col2 = new QVBoxLayout();
        col2->setSpacing(2);
        col2->addWidget(mkLabel(
            QString::fromStdString(c->name) + (winner ? "  [WINNER]" : ""),
            14, C_TEXT, true));
        col2->addWidget(mkLabel(QString::fromStdString(c->party), 11, C_SUBTLE));
        row->addLayout(col2, 1);

        // Mini bar
        auto* bar = new QProgressBar();
        bar->setRange(0, std::max(total,1));
        bar->setValue(c->votes);
        bar->setTextVisible(false);
        bar->setFixedSize(80,5);
        bar->setStyleSheet(QString(
                               "QProgressBar{background:#1F2937;border-radius:2px;border:none;}"
                               "QProgressBar::chunk{background:%1;border-radius:2px;}").arg(col));
        auto* barWrap = new QVBoxLayout();
        barWrap->setAlignment(Qt::AlignCenter);
        barWrap->addWidget(bar);
        row->addLayout(barWrap);

        // Vote count
        auto* vl = mkLabel(QString::number(c->votes), 24, col, true);
        vl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        vl->setFixedWidth(50);
        row->addWidget(vl);

        QString brdCol = winner ? QString(C_YELLOW)
                                : (i<3 ? rankColors[i] : QString(C_BORDER));
        QString bgCol  = winner ? "rgba(255,215,0,0.06)" : QString(C_CARD2);

        auto* frame = new QFrame();
        frame->setLayout(row);
        frame->setStyleSheet(QString(
                                 "QFrame{background:%1;border-radius:12px;border:1.5px solid %2;}")
                                 .arg(bgCol, brdCol));
        leaderboardLayout->addWidget(frame);
    }
    leaderboardLayout->addStretch();
}

// ─────────────────────────────────────────────
//  REFRESH / STATS / LOG
// ─────────────────────────────────────────────
void MainWindow::refreshAll()
{
    buildCandidateCards();
    buildLeaderboard();

    queueList->clear();
    auto snap = voterQueue.snapshot();
    if (snap.empty()) {
        auto* it = new QListWidgetItem("   Queue is empty...");
        it->setForeground(QColor(C_SUBTLE));
        queueList->addItem(it);
    } else {
        for (auto* v : snap) {
            Candidate* c = candidates.findById(v->candidateId);
            QString cname = c ? QString::fromStdString(c->name) : "Unknown";
            auto* it = new QListWidgetItem(
                QString("   %1   ->   %2  (ID: %3)")
                    .arg(QString::fromStdString(v->voterId), cname)
                    .arg(v->candidateId));
            it->setForeground(QColor(C_ORANGE));
            queueList->addItem(it);
        }
    }
    updateStats();
}

void MainWindow::updateStats()
{
    int total = 0;
    Candidate* c = candidates.head;
    while (c) { total += c->votes; c = c->next; }
    statVotes->setText(QString::number(total));
    statQueue->setText(QString::number(voterQueue.size));
    statRejected->setText(QString::number(rejectedCount));
}

void MainWindow::addLog(const QString& msg, const QString& color)
{
    auto* it = new QListWidgetItem("  " + msg);
    it->setForeground(QColor(color));
    QColor bg = QColor(color); bg.setAlpha(28);
    it->setBackground(bg);
    logList->addItem(it);
    logList->scrollToBottom();
}

// ─────────────────────────────────────────────
//  SLOTS
// ─────────────────────────────────────────────
void MainWindow::enqueueVoter()
{
    QString vid = voterInput->text().trimmed().toUpper();
    if (vid.isEmpty()) {
        addLog("Please enter a Voter ID.", C_YELLOW); return;
    }
    if (selectedCandidateId == -1) {
        addLog("Please select a candidate card first!", C_YELLOW); return;
    }
    voterQueue.enqueue(vid.toStdString(), selectedCandidateId);
    Candidate* c = candidates.findById(selectedCandidateId);
    addLog(QString("%1  joined the queue  ->  %2")
               .arg(vid, QString::fromStdString(c->name)), C_CYAN);
    voterInput->clear();
    refreshAll();
}

void MainWindow::processNext()
{
    if (voterQueue.isEmpty()) {
        addLog("Queue is empty — nothing to process.", C_YELLOW); return;
    }
    VoterNode voter = voterQueue.dequeue();
    QString   vid   = QString::fromStdString(voter.voterId);

    if (voterTable.hasVoted(voter.voterId)) {
        rejectedCount++;
        addLog(QString("REJECTED: %1 has already voted!  [Hash Table]").arg(vid), C_RED);
        refreshAll(); return;
    }
    if (!candidateBST.search(voter.candidateId)) {
        rejectedCount++;
        addLog(QString("REJECTED: Candidate ID %1 not found!  [BST]")
                   .arg(voter.candidateId), C_RED);
        refreshAll(); return;
    }

    Candidate* c = candidates.findById(voter.candidateId);
    c->votes++;
    voterTable.markVoted(voter.voterId);
    voteStack.push(voter.voterId, voter.candidateId);

    addLog(QString("VOTE CAST: %1  ->  %2  (%3)")
               .arg(vid, QString::fromStdString(c->name),
                    QString::fromStdString(c->party)), C_GREEN);
    refreshAll();
}

void MainWindow::processAll()
{
    if (voterQueue.isEmpty()) {
        addLog("Queue is empty.", C_YELLOW); return;
    }
    int n = voterQueue.size;
    for (int i = 0; i < n; i++) processNext();
}

void MainWindow::undoLast()
{
    if (voteStack.isEmpty()) {
        addLog("No votes to undo.  [Stack empty]", C_YELLOW); return;
    }
    VoteRecord last = voteStack.pop();
    Candidate* c    = candidates.findById(last.candidateId);
    if (c && c->votes > 0) {
        c->votes--;
        voterTable.remove(last.voterId);
        addLog(QString("UNDO: Vote by %1 for %2 reversed.  [Stack]")
                   .arg(QString::fromStdString(last.voterId),
                        QString::fromStdString(c->name)), C_PINK);
        refreshAll();
    }
}
