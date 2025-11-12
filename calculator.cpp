#include "calculator.h"
#include <QGridLayout> // 必須包含 QGridLayout
#include <QVariant>    // 必須包含 QVariant，因為您使用 property("buttonText")
#include <QLayout>     // 保持 QLayout 引用，以使用 QLayout::SetFixedSize

// 為了簡化,定義一個結構來儲存按鈕配置 (保持在 .cpp 頂層，如您原始碼所示)
struct ButtonConfig {
    const char *text;
    const char *slotName;
    int row;
    int col;
    int rowSpan;
    int colSpan;
};

// Constructor
calculator::calculator(QWidget *parent)
    : QDialog(parent), sumInMemory(0.0), waitingForOperand(true)
{
    setWindowTitle("加大版計算機");

    // --- 1. 顯示螢幕 (Display) 設定 (大幅加大) ---
    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);

    QFont font = display->font();
    // 大幅增加字體大小 (從原始的 +20 增加到 +30)
    font.setPointSize(font.pointSize() + 30);
    display->setFont(font);
    // 固定顯示屏高度，使其與按鈕比例協調
    display->setFixedHeight(120);

    // --- 2. 按鈕創建與佈局 ---
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    // 顯示螢幕
    mainLayout->addWidget(display, 0, 0, 1, 4);

    // 按鈕數據定義 (與您原始碼保持一致)
    const ButtonConfig buttons[] = {
                                    {"\u2190", "clearClicked()", 1, 0, 1, 1}, // 退格鍵
                                    {"/", "binaryOperatorClicked()", 1, 1, 1, 1},
                                    {"*", "binaryOperatorClicked()", 1, 2, 1, 1},
                                    {"-", "binaryOperatorClicked()", 1, 3, 1, 1},
                                    {"7", "digitClicked()", 2, 0, 1, 1},
                                    {"8", "digitClicked()", 2, 1, 1, 1},
                                    {"9", "digitClicked()", 2, 2, 1, 1},
                                    {"+", "binaryOperatorClicked()", 2, 3, 2, 1}, // + 號跨兩行
                                    {"4", "digitClicked()", 3, 0, 1, 1},
                                    {"5", "digitClicked()", 3, 1, 1, 1},
                                    {"6", "digitClicked()", 3, 2, 1, 1},
                                    {"1", "digitClicked()", 4, 0, 1, 1},
                                    {"2", "digitClicked()", 4, 1, 1, 1},
                                    {"3", "digitClicked()", 4, 2, 1, 1},
                                    {"=", "equalsClicked()", 4, 3, 2, 1}, // = 號跨兩行
                                    {"0", "digitClicked()", 5, 0, 1, 2}, // 0 號跨兩列
                                    {".", "digitClicked()", 5, 2, 1, 1},
                                    };

    // 基礎按鈕邊長 (從原始 60 增大到 90)
    const int buttonSize = 90;
    // 按鈕字體大小 (從原始 14 增大到 20)
    const int buttonFontSize = 20;

    // 迴圈遍歷按鈕數據並將其加入佈局
    for (const auto& config : buttons) {
        QString text = config.text;

        // 替換 = 號的顯示符號 (與您原始碼保持一致)
        if (QString(config.text) == "=") {
            text = "\u21B5"; // ⏎ 符號
        }

        QPushButton *button = new QPushButton(text);
        button->setProperty("buttonText", QString(config.text));

        // 根據按鈕跨越的格數設置寬度和高度
        int width = buttonSize * config.colSpan;
        int height = buttonSize * config.rowSpan;

        // 額外處理跨行按鈕的尺寸 (確保 + 和 = 號的高度是兩倍)
        if (QString(config.text) == "+" || QString(config.text) == "=") {
            height = buttonSize * 2;
        }
        if (QString(config.text) == "0") {
            width = buttonSize * 2;
        }

        button->setMinimumSize(width, height);
        button->setMaximumSize(width, height);

        // 設置字體大小
        QFont buttonFont = button->font();
        buttonFont.setPointSize(buttonFontSize);
        button->setFont(buttonFont);

        // 設置樣式以區分運算子和數字鍵
        if (QString("/*-+").contains(config.text)) {
            button->setStyleSheet("background-color: orange; color: white;");
        } else if (QString(config.text) == "=") {
            button->setStyleSheet("background-color: #2ECC71; color: white;"); // 綠色強調
        } else if (QString(config.text) == "\u2190") {
            button->setStyleSheet("background-color: #E74C3C; color: white;"); // 紅色強調
        } else {
            button->setStyleSheet("background-color: #f7f7f7;");
        }

        mainLayout->addWidget(button, config.row, config.col, config.rowSpan, config.colSpan);

        // 直接連接信號與槽 (與您原始碼保持一致)
        if (QString(config.slotName) == "digitClicked()") {
            connect(button, SIGNAL(clicked()), this, SLOT(digitClicked()));
        } else if (QString(config.slotName) == "binaryOperatorClicked()") {
            connect(button, SIGNAL(clicked()), this, SLOT(binaryOperatorClicked()));
        } else if (QString(config.slotName) == "equalsClicked()") {
            connect(button, SIGNAL(clicked()), this, SLOT(equalsClicked()));
        } else if (QString(config.slotName) == "clearClicked()") {
            connect(button, SIGNAL(clicked()), this, SLOT(clearClicked()));
        }
    }

    setLayout(mainLayout);
}

calculator::~calculator() {}

// --- 槽函數實現 (保持功能邏輯與您原始碼完全一致) ---

void calculator::digitClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString digit = clickedButton->property("buttonText").toString();

    if (display->text() == "Error") {
        display->setText("0");
        waitingForOperand = true;
    }

    if (waitingForOperand) {
        if (digit == ".") {
            display->setText("0.");
        } else {
            display->setText(digit);
        }
        waitingForOperand = false;
    } else {
        if (digit == ".") {
            if (!display->text().contains('.')) {
                display->setText(display->text() + digit);
            }
        } else {
            if (display->text() == "0") {
                display->setText(digit);
            } else if (display->text().length() < 15) {
                display->setText(display->text() + digit);
            }
        }
    }
}

void calculator::binaryOperatorClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString clickedOperator = clickedButton->property("buttonText").toString();
    double operandValue = display->text().toDouble();

    if (!pendingOperator.isEmpty()) {
        if (!calculate(operandValue, pendingOperator)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(sumInMemory));
    } else {
        sumInMemory = operandValue;
    }

    pendingOperator = clickedOperator;
    waitingForOperand = true;
}

void calculator::equalsClicked()
{
    double operandValue = display->text().toDouble();

    if (!pendingOperator.isEmpty()) {
        if (!calculate(operandValue, pendingOperator)) {
            abortOperation();
            return;
        }
        pendingOperator.clear();
    } else {
        sumInMemory = operandValue;
    }

    display->setText(QString::number(sumInMemory));
    sumInMemory = 0.0;
    waitingForOperand = true;
}

void calculator::clearClicked()
{
    // 這裡的 clearClicked 處理的是退格鍵 (Back Arrow \u2190) 的邏輯

    // 如果等待操作數，則清除所有狀態 (類似 AC)
    if (waitingForOperand) {
        sumInMemory = 0.0;
        pendingOperator.clear();
        display->setText("0");
        waitingForOperand = true;
    }
    // 否則，執行退格操作
    else {
        QString currentText = display->text();
        if (currentText.length() > 1 && currentText != "0") {
            currentText.chop(1); // 刪除最後一個字符
            display->setText(currentText);
        } else {
            display->setText("0");
            waitingForOperand = true; // 退格到 0 後，下次輸入替換 0
        }
    }
}

bool calculator::calculate(double rightOperand, const QString &pendingOp)
{
    if (pendingOp == "+") {
        sumInMemory += rightOperand;
    } else if (pendingOp == "-") {
        sumInMemory -= rightOperand;
    } else if (pendingOp == "*") {
        sumInMemory *= rightOperand;
    } else if (pendingOp == "/") {
        if (rightOperand == 0.0) {
            return false;
        }
        sumInMemory /= rightOperand;
    }
    return true;
}

void calculator::abortOperation()
{
    clearClicked();
    display->setText("Error");
}
