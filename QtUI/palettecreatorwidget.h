#ifndef PALETTECREATORWIDGET
#define PALETTECREATORWIDGET

#include "ui_palettecreatorwidget.h"

#include <memory>

class PaletteTableView;
class BalloonMessageDialog;
class ResourceManager;

class PaletteCreatorWidget : public QWidget
{
    Q_OBJECT

public:
    enum State
    {
        Create = 0,
        Edit
    };

    PaletteCreatorWidget(QWidget *parent = Q_NULLPTR);
    ~PaletteCreatorWidget();
    
    PaletteTableView* paletteTableView();
    
    void setResourceManager(ResourceManager* manager) { mResourceManager = manager; }
    void setState(State state);

signals:
    void paletteCreated();
    void paletteDiscarded();
    void paletteEdited();
    void editCanceled();
    
public slots:
    void updateButtonUsability();
    
private slots:
    void validateName();
    void finish();
    void discard();
       
private:
    std::unique_ptr<Ui::PaletteCreatorWidget> mUi;
    std::unique_ptr<BalloonMessageDialog> mErrorDialog;
    ResourceManager* mResourceManager = nullptr;
    State mState = Create;
};

#endif
