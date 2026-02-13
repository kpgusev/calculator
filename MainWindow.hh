#pragma once

#include <QMainWindow>

class QTextEdit;
class QLabel;
class QListWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

private slots:
  void onAdd();
  void onSubtract();
  void onMultiply();
  void onDivide();
  void onModulo();
  void onPower();
  void onFactorial();
  void onGcd();
  void onLcm();
  void onCheckPrime();
  void onClear();
  void onCopyResult();
  void onClearHistory();

private:
  void performBinaryOperation(const QString &name, const QString &symbol);
  bool validateInput(const QString &text, const QString &fieldName);
  void appendHistory(const QString &entry);
  QString cleanInput(const QString &raw);

  QTextEdit *m_firstNumberInput;
  QTextEdit *m_secondNumberInput;
  QTextEdit *m_resultOutput;
  QLabel *m_operationLabel;
  QListWidget *m_historyList;
};
