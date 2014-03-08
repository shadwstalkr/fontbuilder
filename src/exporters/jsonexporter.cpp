#include "jsonexporter.h"
#include "../fontconfig.h"

JsonExporter::JsonExporter(QObject *parent)
    : AbstractExporter(parent)
{
    setExtension("json");
}


bool JsonExporter::Export(QByteArray& out)
{
    const FontConfig* cfg = fontConfig();
    int height = metrics().height;

    QMap<QString, QByteArray> font;

    // Font family
    font["family"] = Serialize(cfg->family());

    // Font size
    font["size"] = Serialize(cfg->size());

    // Line height
    font["lineHeight"] = Serialize(height);

    // Texture filename
    QMap<QString, QByteArray> textureInfo;
    textureInfo["filename"] = Serialize(texFilename());
    textureInfo["width"] = Serialize(texWidth());
    textureInfo["height"] = Serialize(texHeight());
    font["texture"] = Serialize(textureInfo, 1);

    QMap<QString, QByteArray> symbolMap;
    foreach(const Symbol& c , symbols()) {
        // id, x, y, width, height, xoffset, yoffset, xadvance

        QMap<QString, QByteArray> symbol;

        QMap<QString, QByteArray> place;
        place["x"] = Serialize(c.placeX);
        place["y"] = Serialize(c.placeY);
        place["w"] = Serialize(c.placeW);
        place["h"] = Serialize(c.placeH);
        symbol["place"] = Serialize(place, 3);

        QMap<QString, QByteArray> offset;
        offset["x"] = Serialize(c.offsetX);
        offset["y"] = Serialize(height - c.offsetY);
        symbol["offset"] = Serialize(offset, 3);

        symbol["advance"] = Serialize(c.advance);

        QMap<QString, QByteArray> kerningMap;
        QMap<uint, int>::const_iterator kerning;
        for(kerning = c.kerning.begin(); kerning != c.kerning.end(); ++kerning) {
            kerningMap[QChar(kerning.key())] = Serialize(kerning.value());
        }

        if(!kerningMap.isEmpty()) {
            symbol["kerning"] = Serialize(kerningMap, 3);
        }

        symbolMap[QChar(c.id)] = Serialize(symbol, 2);
    }

    font["symbols"] = Serialize(symbolMap, 1);

    out += Serialize(font, 0);

    return true;
}

QByteArray JsonExporter::Serialize(QString str) const {
    // escape slashes
    str.replace(QLatin1String("\\"), QLatin1String("\\\\"));

    // escape unicode characters
    QString escaped;
    const ushort* utf16 = str.utf16();

    for(int ii = 0; utf16[ii] != 0; ++ii) {
        if(utf16[ii] < 128) {
            escaped.append(QChar(utf16[ii]));
        } else {
            QString hex = QString::number(utf16[ii], 16)
                .rightJustified(4, QLatin1Char('0'));
            escaped.append("\\u").append(hex);
        }
    }

    // escape quotes and control characters
    escaped.replace(QLatin1String("\""), QLatin1String("\\\""));
    escaped.replace(QLatin1String("\b"), QLatin1String("\\b"));
    escaped.replace(QLatin1String("\t"), QLatin1String("\\t"));
    escaped.replace(QLatin1String("\f"), QLatin1String("\\f"));
    escaped.replace(QLatin1String("\n"), QLatin1String("\\n"));
    escaped.replace(QLatin1String("\r"), QLatin1String("\\r"));

    return QString("\"%1\"").arg(escaped).toUtf8();
}

QByteArray JsonExporter::Serialize(int number) const {
    return QString::number(number).toUtf8();
}

QByteArray JsonExporter::Serialize(const QMap<QString, QByteArray>& map, int indentLevel) const {
    QByteArray out;
    indentLevel *= 2;

    if(indentLevel > 0) {
        out.append("\n");
    }
    out.append(QString(" ").repeated(indentLevel * 2))
        .append("{ ");

    bool first = true;
    QMap<QString, QByteArray>::const_iterator it;
    for(it = map.begin(); it != map.end(); ++it) {
        if(!first) {
            out += ",\n" + QString(" ").repeated((indentLevel + 1) * 2);
        }
        first = false;
        out.append(Serialize(it.key())).append(": ").append(it.value());
    }

    out.append("\n")
        .append(QString(" ").repeated(indentLevel * 2))
        .append("}");

    return out;
}

AbstractExporter* JsonExporterFactoryFunc (QObject* parent) {
    return new JsonExporter(parent);
}
