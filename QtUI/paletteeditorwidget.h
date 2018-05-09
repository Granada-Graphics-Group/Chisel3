#ifndef PALETTEEDITORWIDGET_H
#define PALETTEEDITORWIDGET_H

#include "ui_paletteeditorwidget.h"

#include <memory>

class PaletteTableView;

class PaletteEditorWidget : public QWidget
{
    Q_OBJECT

public:
    PaletteEditorWidget(QWidget *parent = Q_NULLPTR);
    ~PaletteEditorWidget();
    
    PaletteTableView* paletteTableView();
    void setInterpolationButtonChecked(bool checked) { mUi->intepolationButton->setChecked(checked); }
    QToolButton* addToCollectionButton() { return mUi->addToCollectionButton; }
    QToolButton* importButton() { return mUi->importButton; }
    
public slots:
    void updateButtonUsability();
       
private:
    std::unique_ptr<Ui::PaletteEditorWidget> mUi;
};

#endif // PALETTEEDITORWIDGET_H
