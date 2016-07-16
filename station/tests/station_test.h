#include <QObject>

class QByteArray;
class QString;

class StationTest: public QObject
{
  Q_OBJECT

private slots:
  void initTestCase();
  void cleanupTestCase();

  void jsonSaveLoadTest();

private:
  QByteArray getFileContent(const QString& fileName, bool* ok);

};
