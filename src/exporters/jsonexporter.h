#ifndef JSONEXPORTER_H
#define JSONEXPORTER_H

#include "../abstractexporter.h"

class JsonExporter : public AbstractExporter
{
    Q_OBJECT
public:
    explicit JsonExporter(QObject *parent = 0);

    virtual bool Export(QByteArray& out);
signals:

public slots:

private:

    QByteArray Serialize(QString str) const;
    QByteArray Serialize(int number) const;
    QByteArray Serialize(const QMap<QString, QByteArray>& map, int indentLevel) const;

};

#endif
