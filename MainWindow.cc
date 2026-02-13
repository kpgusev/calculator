#include "MainWindow.hh"

#include <QApplication>
#include <QClipboard>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "Integer.hh"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Калькулятор больших чисел");
  resize(900, 600);

  auto *central = new QWidget(this);
  setCentralWidget(central);
  auto *mainLayout = new QHBoxLayout(central);

  auto *leftLayout = new QVBoxLayout;

  leftLayout->addWidget(new QLabel("Число 1:"));
  m_firstNumberInput = new QTextEdit;
  m_firstNumberInput->setMaximumHeight(70);
  leftLayout->addWidget(m_firstNumberInput);

  leftLayout->addWidget(new QLabel("Число 2:"));
  m_secondNumberInput = new QTextEdit;
  m_secondNumberInput->setMaximumHeight(70);
  leftLayout->addWidget(m_secondNumberInput);

  m_operationLabel = new QLabel("Операция: -");
  leftLayout->addWidget(m_operationLabel);

  auto *group = new QGroupBox("Операции");
  auto *grid = new QGridLayout(group);

  auto *buttonAdd = new QPushButton("+");
  auto *buttonSubtract = new QPushButton("−");
  auto *buttonMultiply = new QPushButton("*");
  auto *buttonDivide = new QPushButton("/");
  auto *buttonModulo = new QPushButton("%");
  auto *buttonPower = new QPushButton("^");
  auto *buttonFactorial = new QPushButton("N!");
  auto *buttonGcd = new QPushButton("НОД");
  auto *buttonLcm = new QPushButton("НОК");
  auto *buttonPrime = new QPushButton("Простое?");
  auto *buttonClear = new QPushButton("Очистить");
  auto *buttonCopy = new QPushButton("Копировать");

  grid->addWidget(buttonAdd, 0, 0);
  grid->addWidget(buttonSubtract, 0, 1);
  grid->addWidget(buttonMultiply, 0, 2);
  grid->addWidget(buttonDivide, 1, 0);
  grid->addWidget(buttonModulo, 1, 1);
  grid->addWidget(buttonPower, 1, 2);
  grid->addWidget(buttonFactorial, 2, 0);
  grid->addWidget(buttonGcd, 2, 1);
  grid->addWidget(buttonLcm, 2, 2);
  grid->addWidget(buttonPrime, 3, 0);
  grid->addWidget(buttonClear, 3, 1);
  grid->addWidget(buttonCopy, 3, 2);

  leftLayout->addWidget(group);

  leftLayout->addWidget(new QLabel("Результат:"));
  m_resultOutput = new QTextEdit;
  m_resultOutput->setMaximumHeight(80);
  m_resultOutput->setReadOnly(true);
  leftLayout->addWidget(m_resultOutput);
  leftLayout->addStretch();

  auto *rightLayout = new QVBoxLayout;
  rightLayout->addWidget(new QLabel("История:"));
  m_historyList = new QListWidget;
  rightLayout->addWidget(m_historyList);
  auto *buttonClearHistory = new QPushButton("Очистить историю");
  rightLayout->addWidget(buttonClearHistory);

  mainLayout->addLayout(leftLayout, 3);
  mainLayout->addLayout(rightLayout, 2);

  connect(buttonAdd, &QPushButton::clicked, this, &MainWindow::onAdd);
  connect(buttonSubtract, &QPushButton::clicked, this, &MainWindow::onSubtract);
  connect(buttonMultiply, &QPushButton::clicked, this, &MainWindow::onMultiply);
  connect(buttonDivide, &QPushButton::clicked, this, &MainWindow::onDivide);
  connect(buttonModulo, &QPushButton::clicked, this, &MainWindow::onModulo);
  connect(buttonPower, &QPushButton::clicked, this, &MainWindow::onPower);
  connect(buttonFactorial, &QPushButton::clicked, this,
          &MainWindow::onFactorial);
  connect(buttonGcd, &QPushButton::clicked, this, &MainWindow::onGcd);
  connect(buttonLcm, &QPushButton::clicked, this, &MainWindow::onLcm);
  connect(buttonPrime, &QPushButton::clicked, this, &MainWindow::onCheckPrime);
  connect(buttonClear, &QPushButton::clicked, this, &MainWindow::onClear);
  connect(buttonCopy, &QPushButton::clicked, this, &MainWindow::onCopyResult);
  connect(buttonClearHistory, &QPushButton::clicked, this,
          &MainWindow::onClearHistory);
}

QString MainWindow::cleanInput(const QString &raw) {
  QString cleaned;
  for (const QChar &character : raw)
    if (!character.isSpace())
      cleaned += character;
  return cleaned;
}

bool MainWindow::validateInput(const QString &text, const QString &fieldName) {
  if (text.isEmpty()) {
    QMessageBox::warning(this, "Ошибка", fieldName + " не заполнено.");
    return false;
  }
  int start = 0;
  if (text[0] == '-' || text[0] == '+')
    start = 1;
  if (start >= text.size()) {
    QMessageBox::warning(this, "Ошибка", fieldName + ": введите цифры.");
    return false;
  }
  for (int index = start; index < text.size(); ++index) {
    if (!text[index].isDigit()) {
      QMessageBox::warning(this, "Ошибка",
                           fieldName + " содержит недопустимые символы.");
      return false;
    }
  }
  return true;
}

void MainWindow::appendHistory(const QString &entry) {
  m_historyList->insertItem(0, entry);
}

void MainWindow::performBinaryOperation(const QString &name,
                                        const QString &symbol) {
  QString first = cleanInput(m_firstNumberInput->toPlainText());
  QString second = cleanInput(m_secondNumberInput->toPlainText());
  if (!validateInput(first, "Число 1") || !validateInput(second, "Число 2"))
    return;

  m_operationLabel->setText("Операция: " + name);

  try {
    Integer numberFirst(first.toStdString());
    Integer numberSecond(second.toStdString());
    Integer result(0LL);

    if (symbol == "+")
      result = numberFirst + numberSecond;
    else if (symbol == "-")
      result = numberFirst - numberSecond;
    else if (symbol == "*")
      result = numberFirst * numberSecond;
    else if (symbol == "/")
      result = numberFirst / numberSecond;
    else if (symbol == "%")
      result = numberFirst % numberSecond;
    else if (symbol == "^")
      result = power(numberFirst, numberSecond);

    QString resultString = QString::fromStdString(result.toString());
    m_resultOutput->setText(resultString);
    appendHistory(first + " " + symbol + " " + second + " = " + resultString);
  } catch (const std::exception &exception) {
    QMessageBox::critical(this, "Ошибка", QString::fromUtf8(exception.what()));
  }
}

void MainWindow::onAdd() { performBinaryOperation("Сложение", "+"); }
void MainWindow::onSubtract() { performBinaryOperation("Вычитание", "-"); }
void MainWindow::onMultiply() { performBinaryOperation("Умножение", "*"); }
void MainWindow::onDivide() { performBinaryOperation("Деление", "/"); }
void MainWindow::onModulo() { performBinaryOperation("Остаток", "%"); }
void MainWindow::onPower() { performBinaryOperation("Степень", "^"); }

void MainWindow::onFactorial() {
  QString text = cleanInput(m_firstNumberInput->toPlainText());
  if (!validateInput(text, "Число 1"))
    return;
  m_operationLabel->setText("Операция: Факториал");
  try {
    Integer number(text.toStdString());
    Integer result = factorial(number);
    QString resultString = QString::fromStdString(result.toString());
    m_resultOutput->setText(resultString);
    appendHistory(text + "! = " + resultString);
  } catch (const std::exception &exception) {
    QMessageBox::critical(this, "Ошибка", QString::fromUtf8(exception.what()));
  }
}

void MainWindow::onGcd() {
  QString first = cleanInput(m_firstNumberInput->toPlainText());
  QString second = cleanInput(m_secondNumberInput->toPlainText());
  if (!validateInput(first, "Число 1") || !validateInput(second, "Число 2"))
    return;
  m_operationLabel->setText("Операция: НОД");
  try {
    Integer numberFirst(first.toStdString());
    Integer numberSecond(second.toStdString());
    Integer result = gcd(numberFirst, numberSecond);
    QString resultString = QString::fromStdString(result.toString());
    m_resultOutput->setText(resultString);
    appendHistory("НОД(" + first + ", " + second + ") = " + resultString);
  } catch (const std::exception &exception) {
    QMessageBox::critical(this, "Ошибка", QString::fromUtf8(exception.what()));
  }
}

void MainWindow::onLcm() {
  QString first = cleanInput(m_firstNumberInput->toPlainText());
  QString second = cleanInput(m_secondNumberInput->toPlainText());
  if (!validateInput(first, "Число 1") || !validateInput(second, "Число 2"))
    return;
  m_operationLabel->setText("Операция: НОК");
  try {
    Integer numberFirst(first.toStdString());
    Integer numberSecond(second.toStdString());
    Integer result = lcm(numberFirst, numberSecond);
    QString resultString = QString::fromStdString(result.toString());
    m_resultOutput->setText(resultString);
    appendHistory("НОК(" + first + ", " + second + ") = " + resultString);
  } catch (const std::exception &exception) {
    QMessageBox::critical(this, "Ошибка", QString::fromUtf8(exception.what()));
  }
}

void MainWindow::onCheckPrime() {
  QString text = cleanInput(m_firstNumberInput->toPlainText());
  if (!validateInput(text, "Число 1"))
    return;
  m_operationLabel->setText("Операция: Проверка простоты");
  try {
    Integer number(text.toStdString());
    bool prime = isPrime(number);
    QString answer = prime ? "Простое" : "Составное";
    m_resultOutput->setText(answer);
    appendHistory(text + ": " + answer);
    QMessageBox::information(this, "Результат", answer);
  } catch (const std::exception &exception) {
    QMessageBox::critical(this, "Ошибка", QString::fromUtf8(exception.what()));
  }
}

void MainWindow::onClear() {
  m_firstNumberInput->clear();
  m_secondNumberInput->clear();
  m_resultOutput->clear();
  m_operationLabel->setText("Операция: -");
}

void MainWindow::onCopyResult() {
  QString text = m_resultOutput->toPlainText();
  if (text.isEmpty()) {
    QMessageBox::information(this, "Копирование", "Результат пуст.");
    return;
  }
  QApplication::clipboard()->setText(text);
  QMessageBox::information(this, "Копирование", "Скопировано в буфер обмена.");
}

void MainWindow::onClearHistory() { m_historyList->clear(); }
