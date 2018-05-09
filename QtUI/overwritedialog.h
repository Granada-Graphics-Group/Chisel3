#ifndef OVERWRITEDIALOG_H
#define OVERWRITEDIALOG_H

#include <QDialog>
#include <QFileInfo>
#include <QLineEdit>

class QPushButton;

class OverwriteDialog : public QDialog
{
public:    
    OverwriteDialog(const QFileInfo& source, const QFileInfo& destination, const QStringList& resourceList, std::string resource = "file", QWidget *parent = Q_NULLPTR);

    bool renamed() const { return value == 1; }
    bool skipped() const { return value == -1; }
    bool overwritten() const { return value == 0; }
    QString newName() const { return mNewNameEdit->text(); }
    
private slots:
    void renameSource();
    void skip();
    
private:
    QFileInfo mSource;
    QFileInfo mDestination;
    
    QStringList mResourceList;
    
    int value = 0;
    
    QLineEdit* mNewNameEdit;
    QPushButton* mRenameButton;
    QPushButton* mNewNameButton;
    QPushButton* mOverwriteButton;
    QPushButton* mSkipButton;
    QPushButton* mCancelButton;
};

#endif
