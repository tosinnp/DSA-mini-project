#include "mainwindow.h"
#include <QApplication>
#include <QScrollBar>
#include <QString>
#include <QStringList>
#include <QColor>
#include <QGraphicsDropShadowEffect>
#include <algorithm>

// ─────────────────────────────────────────────
//  COLOR PALETTE  (dark, refined, gov-grade)
// ─────────────────────────────────────────────
#define C_BG       "#0A0E17"
#define C_SURFACE  "#111827"
#define C_CARD     "#141D2E"
#define C_CARD2    "#1A2540"
#define C_BORDER   "#1E2D45"
#define C_BORDER2  "#253450"

#define C_RED      "#E63946"
#define C_PINK     "#F472B6"
#define C_CYAN     "#38BDF8"
#define C_GREEN    "#34D399"
#define C_ORANGE   "#FB923C"
#define C_YELLOW   "#FBBF24"
#define C_PURPLE   "#A78BFA"
#define C_BLUE     "#60A5FA"

#define C_TEXT     "#F1F5F9"
#define C_TEXT2    "#CBD5E1"
#define C_SUBTLE   "#64748B"
#define C_MUTED    "#334155"

// ─────────────────────────────────────────────
//  FONT STACK
// ─────────────────────────────────────────────
#define FONT_MONO  "\"Cascadia Code\", \"Fira Code\", \"Courier New\", monospace"
#define FONT_UI    "\"Segoe UI\", \"SF Pro Display\", \"Helvetica Neue\", sans-serif"

// ─────────────────────────────────────────────
//  WIDGET HELPERS
// ─────────────────────────────────────────────
static QLabel* mkLabel(const QString& t, int sz,
                       const QString& col = C_TEXT, bool bold = false,
                       const QString& font = FONT_UI)
{
    auto* l = new QLabel(t);
    l->setStyleSheet(
        QString("color:%1;font-size:%2px;font-weight:%3;"
                "font-family:%4;background:transparent;letter-spacing:0.2px;")
            .arg(col).arg(sz).arg(bold ? "700" : "400").arg(font));
    l->setWordWrap(true);
    return l;
}

static QPushButton* mkBtn(const QString& text,
                          const QString& bg,
                          const QString& fg    = "#ffffff",
                          const QString& hover = "",
                          bool outlined = false)
{
    auto* b = new QPushButton(text);
    QString hov = hover.isEmpty() ? bg : hover;
    QString style;
    if (outlined) {
        style = QString(R"(
            QPushButton {
                background: transparent; color:%2;
                border: 1.5px solid %1; border-radius:8px;
                padding:8px 16px; font-size:12px;
                font-weight:600; font-family:)" FONT_UI R"(;
                letter-spacing:0.5px;
            }
            QPushButton:hover { background:%1; color:#0A0E17; }
            QPushButton:pressed { padding:9px 15px 7px 17px; }
        )").arg(bg, fg);
    } else {
        style = QString(R"(
            QPushButton {
                background:%1; color:%2;
                border:none; border-radius:8px;
                padding:8px 16px; font-size:12px;
                font-weight:600; font-family:)" FONT_UI R"(;
                letter-spacing:0.5px;
            }
            QPushButton:hover   { background:%3; }
            QPushButton:pressed { padding:9px 15px 7px 17px; }
        )").arg(bg, fg, hov);
    }
    b->setStyleSheet(style);
    b->setCursor(Qt::PointingHandCursor);
    return b;
}

static QFrame* mkCard(QLayout* lay = nullptr,
                      const QString& bg = C_CARD,
                      const QString& brd = C_BORDER,
                      int radius = 12,
                      int padH = 16, int padV = 14)
{
    auto* f = new QFrame();
    f->setStyleSheet(
        QString("QFrame{background:%1;border-radius:%2px;border:1px solid %3;}")
            .arg(bg).arg(radius).arg(brd));
    if (lay) {
        lay->setContentsMargins(padH, padV, padH, padV);
        f->setLayout(lay);
    }
    return f;
}

static QFrame* mkHLine(const QString& col = C_BORDER) {
    auto* f = new QFrame();
    f->setFrameShape(QFrame::HLine);
    f->setStyleSheet(
        QString("color:%1;background:%1;border:none;max-height:1px;min-height:1px;")
            .arg(col));
    return f;
}

static QLabel* mkTag(const QString& text, const QString& color) {
    auto* l = new QLabel(text);
    QColor c(color);
    l->setStyleSheet(QString(
                         "color:%1;"
                         "background:rgba(%2,%3,%4,0.15);"
                         "border:1px solid rgba(%2,%3,%4,0.35);"
                         "border-radius:5px;"
                         "font-size:10px;font-weight:600;"
                         "font-family:" FONT_UI ";"
                         "letter-spacing:1px;"
                         "padding:2px 8px;")
                         .arg(color).arg(c.red()).arg(c.green()).arg(c.blue()));
    l->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    return l;
}

// ─────────────────────────────────────────────
//  CONSTRUCTOR
// ─────────────────────────────────────────────
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowTitle("Nepal Election — Vote Tallying System | COMP 202 DSA · KU");
    setMinimumSize(1100, 750);
    resize(1280, 860);

    qApp->setStyle("Fusion");
    QPalette pal;
    pal.setColor(QPalette::Window,      QColor(C_BG));
    pal.setColor(QPalette::WindowText,  QColor(C_TEXT));
    pal.setColor(QPalette::Base,        QColor(C_CARD));
    pal.setColor(QPalette::Text,        QColor(C_TEXT));
    pal.setColor(QPalette::Button,      QColor(C_CARD));
    pal.setColor(QPalette::ButtonText,  QColor(C_TEXT));
    pal.setColor(QPalette::Highlight,   QColor(C_CYAN));
    qApp->setPalette(pal);

    setStyleSheet(QString(R"(
        QMainWindow { background:%1; }
        QToolTip {
            background:%2; color:%3;
            border:1px solid %4; border-radius:6px;
            font-size:11px; padding:4px 8px;
        }
        QScrollBar:vertical {
            background:transparent; width:5px; border-radius:3px; margin:0;
        }
        QScrollBar::handle:vertical {
            background:%4; border-radius:3px; min-height:24px;
        }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical { height:0; }
        QScrollBar:horizontal { height:0; }
    )").arg(C_BG, C_CARD2, C_TEXT, C_BORDER2));

    // Register candidates
    struct CD { int id; const char* name; const char* party; const char* color; };
    for (auto& d : std::vector<CD>{
                                   {101, "Ram Bahadur",    "Nepali Congress", C_CYAN},
                                   {102, "K.P. Sharma Oli","CPN-UML",         C_GREEN},
                                   {103, "Balendra Shah",  "RSP Nepal",       C_ORANGE},
                                   {104, "Amrit Rasaili",  "Independent",     C_PURPLE},
                                   })
    {
        candidates.add(d.id, d.name, d.party, d.color);
        candidateBST.insert(d.id);
    }

    buildUI();
    refreshAll();
    addLog("Election system initialized and ready.", C_GREEN);
    addLog("Select a candidate → Enter Voter ID → Join Queue → Process.", C_CYAN);
}

MainWindow::~MainWindow() {}

// ─────────────────────────────────────────────
//  BUILD UI
// ─────────────────────────────────────────────
void MainWindow::buildUI()
{
    auto* central = new QWidget();
    setCentralWidget(central);
    central->setStyleSheet(QString("background:%1;").arg(C_BG));

    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(24, 20, 24, 20);
    root->setSpacing(16);

    // ── HEADER ───────────────────────────────
    {
        auto* header = new QHBoxLayout();
        header->setSpacing(20);

        // Left: branding
        auto* brand = new QVBoxLayout();
        brand->setSpacing(6);

        // Flag accent bar
        auto* flagBar = new QFrame();
        flagBar->setFixedHeight(3);
        flagBar->setStyleSheet(
            "QFrame{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
            "stop:0 #E63946,stop:0.48 #E63946,"
            "stop:0.52 #1D3A6B,stop:1 #1D3A6B);"
            "border-radius:2px;border:none;}");
        brand->addWidget(flagBar);

        auto* tagRow = new QHBoxLayout();
        tagRow->setSpacing(8);
        tagRow->addWidget(mkTag("LIVE", C_RED));
        tagRow->addWidget(mkTag("COMP 202 · DSA", C_CYAN));
        tagRow->addWidget(mkTag("KATHMANDU UNIVERSITY", C_SUBTLE));
        tagRow->addStretch();
        brand->addLayout(tagRow);

        auto* titleL = new QLabel("Nepal Election\nVote Tallying System");
        titleL->setStyleSheet(QString(
                                  "color:%1;font-size:26px;font-weight:700;"
                                  "font-family:" FONT_UI ";letter-spacing:-0.5px;"
                                  "line-height:1.2;background:transparent;").arg(C_TEXT));
        brand->addWidget(titleL);

        auto* subL = mkLabel(
            "Linked List  ·  Queue  ·  Stack  ·  BST  ·  Hash Table  ·  Selection Sort",
            11, C_SUBTLE);
        brand->addWidget(subL);

        header->addLayout(brand, 1);

        // Right: stat cards
        auto mkStat = [&](const QString& label, const QString& accentColor) -> QLabel* {
            auto* vl = new QVBoxLayout();
            vl->setSpacing(4);
            vl->setContentsMargins(20, 16, 20, 16);
            vl->setAlignment(Qt::AlignCenter);

            auto* num = new QLabel("0");
            QColor ac(accentColor);
            num->setStyleSheet(QString(
                                   "color:%1;font-size:32px;font-weight:700;"
                                   "font-family:" FONT_MONO ";background:transparent;").arg(accentColor));
            num->setAlignment(Qt::AlignCenter);

            auto* sub = mkLabel(label, 9, C_SUBTLE);
            sub->setAlignment(Qt::AlignCenter);
            sub->setStyleSheet(sub->styleSheet() + "letter-spacing:1.8px;");

            vl->addWidget(num);
            vl->addWidget(sub);

            auto* card = new QFrame();
            card->setLayout(vl);
            card->setFixedSize(116, 82);
            card->setStyleSheet(QString(
                                    "QFrame{background:%1;"
                                    "border-radius:10px;"
                                    "border:1px solid rgba(%2,%3,%4,0.25);}")
                                    .arg(C_CARD2).arg(ac.red()).arg(ac.green()).arg(ac.blue()));
            header->addWidget(card);
            return num;
        };

        statVotes    = mkStat("VOTES CAST", C_CYAN);
        statQueue    = mkStat("IN QUEUE",   C_ORANGE);
        statRejected = mkStat("REJECTED",   C_RED);

        root->addLayout(header);
    }

    root->addWidget(mkHLine());

    // ── BODY ─────────────────────────────────
    auto* body = new QHBoxLayout();
    body->setSpacing(16);

    // ── LEFT COLUMN ──────────────────────────
    auto* leftCol = new QVBoxLayout();
    leftCol->setSpacing(14);

    // Section header: Candidates
    {
        auto* secRow = new QHBoxLayout();
        auto* secTitle = mkLabel("CANDIDATES", 10, C_SUBTLE);
        secTitle->setStyleSheet(secTitle->styleSheet() + "letter-spacing:2px;");
        secRow->addWidget(secTitle);
        secRow->addStretch();
        secRow->addWidget(mkLabel("Click a card to select", 10, C_MUTED));
        leftCol->addLayout(secRow);
    }

    // Candidate scroll area
    {
        auto* scroll = new QScrollArea();
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setStyleSheet("QScrollArea{border:none;background:transparent;}");
        auto* container = new QWidget();
        container->setStyleSheet("background:transparent;");
        candidatesLayout = new QVBoxLayout(container);
        candidatesLayout->setSpacing(8);
        candidatesLayout->setContentsMargins(0, 0, 4, 0);
        scroll->setWidget(container);
        scroll->setMinimumHeight(290);
        leftCol->addWidget(scroll, 1);
    }

    // Vote panel
    {
        leftCol->addWidget(mkHLine());

        auto* secTitle = mkLabel("CAST VOTE", 10, C_SUBTLE);
        secTitle->setStyleSheet(secTitle->styleSheet() + "letter-spacing:2px;");
        leftCol->addWidget(secTitle);

        auto* pnl = new QVBoxLayout();
        pnl->setSpacing(10);

        voterInput = new QLineEdit();
        voterInput->setPlaceholderText("Voter ID  —  e.g.  V-001,  V-042 ...");
        voterInput->setMinimumHeight(40);
        voterInput->setStyleSheet(QString(R"(
            QLineEdit {
                background:%1; color:%2;
                border:1px solid %3; border-radius:8px;
                padding:0 14px; font-size:13px;
                font-family:)" FONT_MONO R"(;
            }
            QLineEdit:focus { border-color:%4; }
            QLineEdit:hover { border-color:%5; }
        )").arg(C_BG, C_TEXT, C_BORDER, C_CYAN, C_BORDER2));
        pnl->addWidget(voterInput);

        auto* btnRow = new QHBoxLayout();
        btnRow->setSpacing(8);

        auto* bJoin = mkBtn("＋  Join Queue",    C_RED,   "#fff",    "#C0303C");
        auto* bNext = mkBtn("▶  Process Next",  C_CARD2, C_CYAN,   C_CARD);
        auto* bAll  = mkBtn("⏩  Process All",  C_CARD2, C_YELLOW,  C_CARD);
        auto* bUndo = mkBtn("↩  Undo",          C_CARD2, C_PINK,   C_CARD);

        bJoin->setToolTip("Add voter to the processing queue");
        bNext->setToolTip("Process one voter from the queue");
        bAll->setToolTip("Process all voters in the queue");
        bUndo->setToolTip("Undo last successful vote");

        btnRow->addWidget(bJoin, 5);
        btnRow->addWidget(bNext, 4);
        btnRow->addWidget(bAll,  4);
        btnRow->addWidget(bUndo, 3);
        pnl->addLayout(btnRow);

        leftCol->addWidget(mkCard(pnl, C_CARD2, C_BORDER, 12, 14, 14));

        connect(bJoin, &QPushButton::clicked, this, &MainWindow::enqueueVoter);
        connect(bNext, &QPushButton::clicked, this, &MainWindow::processNext);
        connect(bAll,  &QPushButton::clicked, this, &MainWindow::processAll);
        connect(bUndo, &QPushButton::clicked, this, &MainWindow::undoLast);
        connect(voterInput, &QLineEdit::returnPressed, this, &MainWindow::enqueueVoter);
    }

    // Queue display
    {
        auto* qHdr = new QHBoxLayout();
        auto* qTitle = mkLabel("VOTER QUEUE", 10, C_SUBTLE);
        qTitle->setStyleSheet(qTitle->styleSheet() + "letter-spacing:2px;");
        qHdr->addWidget(qTitle);
        qHdr->addStretch();
        qHdr->addWidget(mkTag("FIFO", C_ORANGE));
        leftCol->addLayout(qHdr);

        queueList = new QListWidget();
        queueList->setFixedHeight(115);
        queueList->setStyleSheet(QString(R"(
            QListWidget {
                background:%1; border:1px solid %2;
                border-radius:10px; padding:4px 2px;
                font-size:12px; font-family:)" FONT_MONO R"(;
            }
            QListWidget::item {
                color:%3;
                border-radius:6px; padding:5px 10px; margin:2px 4px;
            }
            QListWidget::item:hover   { background:rgba(251,146,60,0.1); }
            QListWidget::item:selected { background:rgba(251,146,60,0.18); }
        )").arg(C_BG, C_BORDER, C_ORANGE));
        leftCol->addWidget(queueList);
    }

    body->addLayout(leftCol, 3);

    // ── RIGHT COLUMN ─────────────────────────
    auto* rightCol = new QVBoxLayout();
    rightCol->setSpacing(14);

    // Leaderboard
    {
        auto* lHdr = new QHBoxLayout();
        auto* lTitle = mkLabel("LIVE LEADERBOARD", 10, C_SUBTLE);
        lTitle->setStyleSheet(lTitle->styleSheet() + "letter-spacing:2px;");
        lHdr->addWidget(lTitle);
        lHdr->addStretch();
        lHdr->addWidget(mkTag("SELECTION SORT", C_YELLOW));
        rightCol->addLayout(lHdr);

        auto* scroll = new QScrollArea();
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setStyleSheet("QScrollArea{border:none;background:transparent;}");
        auto* container = new QWidget();
        container->setStyleSheet("background:transparent;");
        leaderboardLayout = new QVBoxLayout(container);
        leaderboardLayout->setSpacing(7);
        leaderboardLayout->setContentsMargins(0, 0, 4, 0);
        scroll->setWidget(container);
        scroll->setMinimumHeight(260);
        rightCol->addWidget(scroll, 1);
    }

    // Activity log
    {
        rightCol->addWidget(mkHLine());

        auto* logHdr = new QHBoxLayout();
        auto* logTitle = mkLabel("ACTIVITY LOG", 10, C_SUBTLE);
        logTitle->setStyleSheet(logTitle->styleSheet() + "letter-spacing:2px;");
        logHdr->addWidget(logTitle);
        logHdr->addStretch();
        logHdr->addWidget(mkTag("LIVE", C_GREEN));
        rightCol->addLayout(logHdr);

        logList = new QListWidget();
        logList->setStyleSheet(QString(R"(
            QListWidget {
                background:%1; border:1px solid %2;
                border-radius:10px; font-size:12px;
                font-family:)" FONT_MONO R"(; padding:4px 2px;
            }
            QListWidget::item {
                border-radius:6px; padding:5px 10px; margin:2px 4px;
            }
        )").arg(C_BG, C_BORDER));
        logList->setMinimumHeight(200);
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
        QColor  qc(col);

        // Card background
        QString bgStr  = sel
                            ? QString("rgba(%1,%2,%3,0.10)").arg(qc.red()).arg(qc.green()).arg(qc.blue())
                            : QString(C_CARD);
        QString brdStr = sel ? col : QString(C_BORDER);

        auto* row = new QHBoxLayout();
        row->setSpacing(14);
        row->setContentsMargins(14, 12, 14, 12);

        // Avatar circle
        auto* av = new QLabel(QString(QChar(c->name[0])));
        av->setFixedSize(44, 44);
        av->setAlignment(Qt::AlignCenter);
        av->setStyleSheet(QString(
                              "background:rgba(%1,%2,%3,0.20);"
                              "color:%4;border-radius:22px;"
                              "font-size:18px;font-weight:700;"
                              "font-family:" FONT_UI ";border:1.5px solid rgba(%1,%2,%3,0.4);")
                              .arg(qc.red()).arg(qc.green()).arg(qc.blue()).arg(col));
        row->addWidget(av);

        // Info
        auto* info = new QVBoxLayout();
        info->setSpacing(3);

        auto* nameL = mkLabel(QString::fromStdString(c->name), 14, C_TEXT, true);
        auto* partyL = mkLabel(QString::fromStdString(c->party), 11, C_SUBTLE);

        auto* bar = new QProgressBar();
        bar->setRange(0, 100);
        bar->setValue(pct);
        bar->setTextVisible(false);
        bar->setFixedHeight(4);
        bar->setStyleSheet(QString(
                               "QProgressBar{background:%1;border-radius:2px;border:none;}"
                               "QProgressBar::chunk{background:%2;border-radius:2px;}")
                               .arg(C_MUTED).arg(col));

        info->addWidget(nameL);
        info->addWidget(partyL);
        info->addWidget(bar);
        row->addLayout(info, 1);

        // Vote count
        auto* vc = new QVBoxLayout();
        vc->setAlignment(Qt::AlignCenter | Qt::AlignRight);
        vc->setSpacing(2);
        auto* vNum = mkLabel(QString::number(c->votes), 24, col, true, FONT_MONO);
        vNum->setAlignment(Qt::AlignRight);
        auto* vPct = mkLabel(QString::number(pct) + "%", 10, C_SUBTLE, false, FONT_MONO);
        vPct->setAlignment(Qt::AlignRight);
        vc->addWidget(vNum);
        vc->addWidget(vPct);
        row->addLayout(vc);

        // Select button
        int cid = c->id;
        QPushButton* selBtn;
        if (sel) {
            selBtn = mkBtn("✓  Selected", col, C_BG);
        } else {
            selBtn = mkBtn("Select", col, col, "", true);
        }
        selBtn->setFixedWidth(86);
        connect(selBtn, &QPushButton::clicked, this, [this, cid](){
            selectedCandidateId = cid;
            buildCandidateCards();
        });
        row->addWidget(selBtn);

        auto* frame = new QFrame();
        frame->setLayout(row);
        frame->setStyleSheet(QString(
                                 "QFrame{background:%1;border-radius:10px;border:1px solid %2;}")
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

    const QStringList medals     = {"#1", "#2", "#3"};
    const QStringList rankColors = {C_YELLOW, "#C0C0C0", "#CD7F32"};

    int total = 0;
    Candidate* cur = candidates.head;
    while (cur) { total += cur->votes; cur = cur->next; }

    for (int i = 0; i < (int)sorted.size(); i++) {
        auto*   c      = sorted[i];
        QString col    = QString::fromStdString(c->color);
        QColor  qc(col);
        bool    winner = (i == 0 && c->votes > 0);
        int     pct    = total > 0 ? (c->votes * 100 / total) : 0;

        auto* row = new QHBoxLayout();
        row->setSpacing(12);
        row->setContentsMargins(14, 11, 14, 11);

        // Rank badge
        QString rankCol = i < 3 ? rankColors[i] : QString(C_MUTED);
        auto* rankL = mkLabel(i < 3 ? medals[i] : QString::number(i+1)+".", 12, rankCol, true, FONT_MONO);
        rankL->setFixedWidth(34);
        rankL->setAlignment(Qt::AlignCenter);
        row->addWidget(rankL);

        // Avatar dot
        auto* dot = new QFrame();
        dot->setFixedSize(8, 8);
        dot->setStyleSheet(QString("QFrame{background:%1;border-radius:4px;border:none;}").arg(col));
        row->addWidget(dot);

        // Name + party
        auto* col2 = new QVBoxLayout();
        col2->setSpacing(2);
        QString nameStr = QString::fromStdString(c->name);
        if (winner) nameStr += "  ★";
        col2->addWidget(mkLabel(nameStr, 13, winner ? col : QString(C_TEXT), true));
        col2->addWidget(mkLabel(QString::fromStdString(c->party), 10, C_SUBTLE));
        row->addLayout(col2, 1);

        // Progress bar
        auto* bar = new QProgressBar();
        bar->setRange(0, std::max(total, 1));
        bar->setValue(c->votes);
        bar->setTextVisible(false);
        bar->setFixedSize(72, 4);
        bar->setStyleSheet(QString(
                               "QProgressBar{background:%1;border-radius:2px;border:none;}"
                               "QProgressBar::chunk{background:%2;border-radius:2px;}")
                               .arg(C_MUTED).arg(col));
        auto* bw = new QVBoxLayout();
        bw->setAlignment(Qt::AlignVCenter);
        bw->addWidget(bar);
        row->addLayout(bw);

        // Pct + vote
        auto* vc = new QVBoxLayout();
        vc->setSpacing(2);
        vc->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        auto* vNum = mkLabel(QString::number(c->votes), 20, col, true, FONT_MONO);
        vNum->setAlignment(Qt::AlignRight);
        auto* vPct = mkLabel(QString::number(pct)+"%", 10, C_SUBTLE, false, FONT_MONO);
        vPct->setAlignment(Qt::AlignRight);
        vc->addWidget(vNum);
        vc->addWidget(vPct);
        row->addLayout(vc);

        QString brdCol = winner ? QString(C_YELLOW) : (i < 3 ? rankColors[i] : QString(C_BORDER));
        QString bgCol  = winner
                            ? "rgba(251,191,36,0.07)"
                            : QString("rgba(%1,%2,%3,0.04)").arg(qc.red()).arg(qc.green()).arg(qc.blue());

        auto* frame = new QFrame();
        frame->setLayout(row);
        frame->setStyleSheet(QString(
                                 "QFrame{background:%1;border-radius:10px;border:1px solid %2;}")
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
        auto* it = new QListWidgetItem("  Queue is empty...");
        it->setForeground(QColor(C_MUTED));
        queueList->addItem(it);
    } else {
        for (auto* v : snap) {
            Candidate* c = candidates.findById(v->candidateId);
            QString cname = c ? QString::fromStdString(c->name) : "?";
            auto* it = new QListWidgetItem(
                QString("  %1  →  %2  (ID: %3)")
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
    QColor bg = QColor(color);
    bg.setAlpha(20);
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
        addLog("⚠  Please enter a Voter ID.", C_YELLOW); return;
    }
    if (selectedCandidateId == -1) {
        addLog("⚠  Please select a candidate first!", C_YELLOW); return;
    }
    voterQueue.enqueue(vid.toStdString(), selectedCandidateId);
    Candidate* c = candidates.findById(selectedCandidateId);
    addLog(QString("QUEUED   %1  →  %2")
               .arg(vid, QString::fromStdString(c->name)), C_CYAN);
    voterInput->clear();
    refreshAll();
}

void MainWindow::processNext()
{
    if (voterQueue.isEmpty()) {
        addLog("⚠  Queue is empty — nothing to process.", C_YELLOW); return;
    }
    VoterNode voter = voterQueue.dequeue();
    QString   vid   = QString::fromStdString(voter.voterId);

    if (voterTable.hasVoted(voter.voterId)) {
        rejectedCount++;
        addLog(QString("REJECTED   %1  —  already voted  [Hash Table]").arg(vid), C_RED);
        refreshAll(); return;
    }
    if (!candidateBST.search(voter.candidateId)) {
        rejectedCount++;
        addLog(QString("REJECTED   Candidate ID %1 not found  [BST]")
                   .arg(voter.candidateId), C_RED);
        refreshAll(); return;
    }

    Candidate* c = candidates.findById(voter.candidateId);
    c->votes++;
    voterTable.markVoted(voter.voterId);
    voteStack.push(voter.voterId, voter.candidateId);

    addLog(QString("ACCEPTED   %1  →  %2  ·  %3")
               .arg(vid, QString::fromStdString(c->name),
                    QString::fromStdString(c->party)), C_GREEN);
    refreshAll();
}

void MainWindow::processAll()
{
    if (voterQueue.isEmpty()) {
        addLog("⚠  Queue is empty.", C_YELLOW); return;
    }
    int n = voterQueue.size;
    for (int i = 0; i < n; i++) processNext();
}

void MainWindow::undoLast()
{
    if (voteStack.isEmpty()) {
        addLog("⚠  No votes to undo.  [Stack empty]", C_YELLOW); return;
    }
    VoteRecord last = voteStack.pop();
    Candidate* c    = candidates.findById(last.candidateId);
    if (c && c->votes > 0) {
        c->votes--;
        voterTable.remove(last.voterId);
        addLog(QString("UNDONE   %1  →  %2  [Stack]")
                   .arg(QString::fromStdString(last.voterId),
                        QString::fromStdString(c->name)), C_PINK);
        refreshAll();
    }
}
