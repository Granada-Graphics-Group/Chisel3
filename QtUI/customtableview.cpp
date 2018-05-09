#include "customtableview.h"

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>

CustomTableView::CustomTableView(QWidget* parent)
:
    QTableView(parent)
{
    
}


// *** Public Methods *** //

void CustomTableView::copyAll()
{
    auto loc = QLocale::system();
        
    QString selectedText;

    for(auto column = 0; column < model()->columnCount() - 1; ++column)
        selectedText += model()->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString() + QLatin1Char('\t');

    selectedText += model()->headerData(model()->columnCount() - 1, Qt::Horizontal, Qt::DisplayRole).toString() + QLatin1Char('\n');

    auto lastRow = model()->rowCount() - 1;
    auto lastColumn = model()->columnCount() - 1;
    
    for(auto row = 0; row < model()->rowCount(); ++row)
    {        
        for(auto column = 0; column < model()->columnCount(); ++column)
        {               
            auto current = model()->index(row, column);
            auto currentType = static_cast<QMetaType::Type>(model()->data(current).type());
            if(currentType == QMetaType::Double || currentType == QMetaType::Float)
                selectedText += loc.toString(model()->data(current).toDouble());
            else
                selectedText += model()->data(current).toString();

            if(column != lastColumn)
                selectedText += QLatin1Char('\t');            
        }
        
        if(row != lastRow)
            selectedText += QLatin1Char('\n');        
    }

    QApplication::clipboard()->setText(selectedText);    
}


// *** Protected Methods *** //

void CustomTableView::keyPressEvent(QKeyEvent* event)
{
    if(event->matches(QKeySequence::Copy) )
    {
        copy();
    }
    else if(event->matches(QKeySequence::Paste) )
    {
        paste();
    }
    else
        QTableView::keyPressEvent(event);
}



// *** Private Methods *** //

void CustomTableView::copy()
{
    auto loc = QLocale::system();
    auto selection = selectionModel();
    auto indices = selection->selectedIndexes();

    if(indices.isEmpty())
        return;

    QMap<int, bool> selectedColumnsMap;
    foreach (QModelIndex current, indices)
        selectedColumnsMap[current.column()] = true;

    QList<int> selectedColumns = selectedColumnsMap.uniqueKeys();
    int minCol = selectedColumns.first();

    // prepend headers for selected columns
    QString selectedText;

    foreach (int column, selectedColumns)
    {
        selectedText += model()->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
        if (column != selectedColumns.last())
            selectedText += QLatin1Char('\t');
    }
    selectedText += QLatin1Char('\n');

    // QModelIndex::operator < sorts first by row, then by column.
    // this is what we need
    qSort(indices);

    int lastRow = indices.first().row();
    int lastColumn = minCol;

    for(const auto& current: indices)
    {
        if (current.row() != lastRow)
        {
            selectedText += QLatin1Char('\n');
            lastColumn = minCol;
            lastRow = current.row();
        }

        if (current.column() != lastColumn)
        {
            for (int i = 0; i < current.column() - lastColumn; ++i)
                selectedText += QLatin1Char('\t');
            lastColumn = current.column();
        }

        auto currentType = static_cast<QMetaType::Type>(model()->data(current).type());
        if(currentType == QMetaType::Double || currentType == QMetaType::Float)
            selectedText += loc.toString(model()->data(current).toDouble());
        else        
            selectedText += model()->data(current).toString();
    }

    selectedText += QLatin1Char('\n');

    QApplication::clipboard()->setText(selectedText);    
}

void CustomTableView::paste()
{
    QString text = QApplication::clipboard()->text();
    QStringList rowContents = text.split("\n");

    QModelIndex initIndex = selectedIndexes().at(0);
    auto initRow = initIndex.row();
    auto initCol = initIndex.column();

    for (auto i = 0; i < rowContents.size(); ++i) 
    {
        QStringList columnContents = rowContents.at(i).split("\t");

        for (auto j = 0; j < columnContents.size(); ++j)
            model()->setData(model()->index(initRow + i, initCol + j), columnContents.at(j));
    }
}
