#ifndef VALIDATEDDELEGATE_H
#define VALIDATEDDELEGATE_H

#include <QStyledItemDelegate>

class ValidatedDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    
public:
    enum class ValidatorType { Int, Double };
    ValidatedDelegate(QObject *parent = Q_NULLPTR): ValidatedDelegate(ValidatorType::Int, parent){};
    ValidatedDelegate(ValidatorType type, QObject *parent = Q_NULLPTR);
    
    ~ValidatedDelegate();
    
    ValidatorType validatorType() const { return mType; }

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setValidatorType(ValidatorType type) { mType = type; }
    
public slots:
    void rejectedValidation(const QString& text);
    void acceptedValidation();

signals:
    void invalidInput(const QString& text) const;
    void duplicatedInput(const QString& text) const;
    void validInput() const;
    void cancelEdit() const;
    
protected:
    bool eventFilter(QObject *editor, QEvent *event) Q_DECL_OVERRIDE;

private:
    ValidatorType mType;
    
};

#endif // VALIDATEDDELEGATE_H
