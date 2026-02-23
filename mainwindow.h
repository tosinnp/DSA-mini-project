#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <vector>
#include <string>

// ─────────────────────────────────────────────
//  DSA 1: LINKED LIST — Candidate Registry
// ─────────────────────────────────────────────
struct Candidate {
    int         id;
    std::string name;
    std::string party;
    std::string color;
    int         votes;
    Candidate*  next;
};

class CandidateList {
public:
    Candidate* head;
    int        count;
    CandidateList() : head(nullptr), count(0) {}

    void add(int id, const std::string& name, const std::string& party, const std::string& color) {
        Candidate* n = new Candidate{id, name, party, color, 0, nullptr};
        if (!head) { head = n; }
        else { Candidate* c = head; while (c->next) c = c->next; c->next = n; }
        count++;
    }

    Candidate* findById(int id) {
        Candidate* c = head;
        while (c) { if (c->id == id) return c; c = c->next; }
        return nullptr;
    }

    std::vector<Candidate*> toVector() {
        std::vector<Candidate*> v;
        Candidate* c = head;
        while (c) { v.push_back(c); c = c->next; }
        return v;
    }

    ~CandidateList() {
        Candidate* c = head;
        while (c) { Candidate* t = c->next; delete c; c = t; }
    }
};

// ─────────────────────────────────────────────
//  DSA 2: QUEUE — Voter Processing Line
// ─────────────────────────────────────────────
struct VoterNode { std::string voterId; int candidateId; VoterNode* next; };

class VoterQueue {
    VoterNode* front;
    VoterNode* rear;
public:
    int size;
    VoterQueue() : front(nullptr), rear(nullptr), size(0) {}
    bool isEmpty() { return !front; }

    void enqueue(const std::string& vid, int cid) {
        VoterNode* n = new VoterNode{vid, cid, nullptr};
        if (!rear) front = rear = n;
        else { rear->next = n; rear = n; }
        size++;
    }

    VoterNode dequeue() {
        if (isEmpty()) return {"", -1, nullptr};
        VoterNode d = *front;
        VoterNode* t = front;
        front = front->next;
        if (!front) rear = nullptr;
        delete t; size--;
        return d;
    }

    std::vector<VoterNode*> snapshot() {
        std::vector<VoterNode*> v;
        VoterNode* c = front;
        while (c) { v.push_back(c); c = c->next; }
        return v;
    }

    ~VoterQueue() { while (!isEmpty()) dequeue(); }
};

// ─────────────────────────────────────────────
//  DSA 3: STACK — Undo Last Vote
// ─────────────────────────────────────────────
struct VoteRecord { std::string voterId; int candidateId; };
struct StackNode  { VoteRecord data; StackNode* next; };

class VoteStack {
    StackNode* top;
public:
    VoteStack() : top(nullptr) {}
    bool isEmpty() { return !top; }
    void push(const std::string& vid, int cid) {
        top = new StackNode{{vid, cid}, top};
    }
    VoteRecord pop() {
        if (isEmpty()) return {"", -1};
        VoteRecord d = top->data;
        StackNode* t = top; top = top->next; delete t;
        return d;
    }
    ~VoteStack() { while (!isEmpty()) pop(); }
};

// ─────────────────────────────────────────────
//  DSA 4: BST — Validate Candidate ID
// ─────────────────────────────────────────────
struct BSTNode { int id; BSTNode* left; BSTNode* right; };

class CandidateBST {
    BSTNode* root;
    BSTNode* insert(BSTNode* n, int id) {
        if (!n) return new BSTNode{id, nullptr, nullptr};
        if (id < n->id) n->left  = insert(n->left,  id);
        else            n->right = insert(n->right, id);
        return n;
    }
    bool search(BSTNode* n, int id) {
        if (!n) return false;
        if (n->id == id) return true;
        return id < n->id ? search(n->left, id) : search(n->right, id);
    }
    void destroy(BSTNode* n) {
        if (!n) return; destroy(n->left); destroy(n->right); delete n;
    }
public:
    CandidateBST() : root(nullptr) {}
    void insert(int id) { root = insert(root, id); }
    bool search(int id) { return search(root, id); }
    ~CandidateBST()     { destroy(root); }
};

// ─────────────────────────────────────────────
//  DSA 5: HASH TABLE — Prevent Double Voting
// ─────────────────────────────────────────────
const int HASH_SIZE = 101;
struct HNode { std::string id; HNode* next; };

class VoterHashTable {
    HNode* table[HASH_SIZE];
    int hash(const std::string& s) {
        int h = 0; for (char c : s) h = (h * 31 + c) % HASH_SIZE; return h;
    }
public:
    VoterHashTable()  { for (int i = 0; i < HASH_SIZE; i++) table[i] = nullptr; }
    bool hasVoted(const std::string& id) {
        HNode* c = table[hash(id)];
        while (c) { if (c->id == id) return true; c = c->next; }
        return false;
    }
    void markVoted(const std::string& id) {
        int i = hash(id); table[i] = new HNode{id, table[i]};
    }
    void remove(const std::string& id) {
        int i = hash(id); HNode* c = table[i], *p = nullptr;
        while (c) {
            if (c->id == id) {
                if (p) p->next = c->next; else table[i] = c->next;
                delete c; return;
            }
            p = c; c = c->next;
        }
    }
    ~VoterHashTable() {
        for (int i = 0; i < HASH_SIZE; i++) {
            HNode* c = table[i];
            while (c) { HNode* t = c->next; delete c; c = t; }
        }
    }
};

// ─────────────────────────────────────────────
//  DSA 6: SELECTION SORT — Leaderboard
// ─────────────────────────────────────────────
inline void selectionSort(std::vector<Candidate*>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        int mx = i;
        for (int j = i + 1; j < n; j++)
            if (arr[j]->votes > arr[mx]->votes) mx = j;
        std::swap(arr[i], arr[mx]);
    }
}

// ─────────────────────────────────────────────
//  MAIN WINDOW
// ─────────────────────────────────────────────
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void enqueueVoter();
    void processNext();
    void processAll();
    void undoLast();

private:
    // DSA
    CandidateList  candidates;
    VoterQueue     voterQueue;
    VoteStack      voteStack;
    CandidateBST   candidateBST;
    VoterHashTable voterTable;
    int            selectedCandidateId = -1;
    int            rejectedCount       = 0;

    // UI widgets
    QLineEdit*   voterInput;
    QListWidget* logList;
    QListWidget* queueList;
    QVBoxLayout* candidatesLayout;
    QVBoxLayout* leaderboardLayout;
    QLabel*      statVotes;
    QLabel*      statQueue;
    QLabel*      statRejected;

    // Helpers
    void buildUI();
    void refreshAll();
    void buildCandidateCards();
    void buildLeaderboard();
    void buildQueueDisplay();
    void updateStats();
    void addLog(const QString& msg, const QString& color);
};

#endif // MAINWINDOW_H
