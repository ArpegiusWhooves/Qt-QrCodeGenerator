/*
 * Copyright (c) 2023 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <QPainter>
#include <QTextStream>

#include "QrCodeGenerator.h"

/**
 * @brief Default constructor for the QrCodeGenerator class.
 * @param parent QObject parent
 */
QrCodeGenerator::QrCodeGenerator(QObject *parent)
    : QObject(parent)
{
    // No implementation needed for default constructor.
}

/**
 * @brief Generates a QR code image from a given text string.
 * @param data The data to encode in the QR code.
 * @param size The image size for the generated QR code.
 * @param borderSize The size of the border around the QR code.
 * @param errorCorrection The level of error correction to apply.
 * @return QImage representing the generated QR code.
 */
QImage QrCodeGenerator::generateQr(const QString &data, quint16 size, quint16 borderSize,
                                   qrcodegen::QrCode::Ecc errorCorrection)
{
    auto b = data.toUtf8();
    const auto qrCode = qrcodegen::QrCode::encodeText(b.constData(), errorCorrection);
    return qrCodeToImage(qrCode, borderSize, size);
}

/**
     * @brief Paints a QR code from the given data and error correction level.
     * @param painter The QPainter object to use.
     * @param data The QString containing the data to encode in the QR code.
     * @param size The desired width/height of the generated image (default: 500).
     * @param borderSize The desired border width of the generated image (default: 1).
     * @param errorCorrection The desired error correction level (default:
     * qrcodegen::QrCode::Ecc::MEDIUM).
     */
void QrCodeGenerator::paintQr(QPainter *painter, const QString &data, const quint16 size, const quint16 borderSize, qrcodegen::QrCode::Ecc errorCorrection)
{
    auto b = data.toUtf8();
    const auto qrCode = qrcodegen::QrCode::encodeText(b.constData(), errorCorrection);
    qrCodePaint(painter, qrCode, borderSize, size);
}

/**
 * @brief Generates an SVG string representing a QR code.
 * @param data The data to encode in the QR code.
 * @param borderSize The size of the border around the QR code.
 * @param errorCorrection The level of error correction to apply.
 * @return QString containing the SVG representation of the QR code.
 */
QString QrCodeGenerator::generateSvgQr(const QString &data, quint16 borderSize,
                                       qrcodegen::QrCode::Ecc errorCorrection)
{
    auto b = data.toUtf8();
    const auto qrCode = qrcodegen::QrCode::encodeText(b.constData(), errorCorrection);
    return toSvgString(qrCode, borderSize);
}

/**
 * @brief Converts a QR code to its SVG representation as a string.
 * @param qr The QR code to convert.
 * @param border The border size to use.
 * @return QString containing the SVG representation of the QR code.
 */
QString QrCodeGenerator::toSvgString(const qrcodegen::QrCode &qr, quint16 border) const
{
    QString str;
    QTextStream sb(&str);

    sb << R"(<?xml version="1.0" encoding="UTF-8"?>)"
       << R"(<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">)"
       << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1" viewBox="0 0 )"
       << (qr.getSize() + border * 2) << " " << (qr.getSize() + border * 2)
       << R"(" stroke="none"><rect width="100%" height="100%" fill="#FFFFFF"/><path d=")";

    for (int y = 0; y < qr.getSize(); y++)
    {
        for (int x = 0; x < qr.getSize(); x++)
        {
            if (qr.getModule(x, y))
            {
                sb << (x == 0 && y == 0 ? "" : " ") << "M" << (x + border) << "," << (y + border)
                   << "h1v1h-1z";
            }
        }
    }

    sb << R"(" fill="#000000"/></svg>)";
    return str;
}

/**
     * @brief Paints a qrcodegen::QrCode object with a QPainter.
     * @param painter The QPainter object to use.
     * @param qrCode The qrcodegen::QrCode object to convert.
     * @param size The desired width/height of the generated image.
     * @param borderSize The desired border width of the generated image.
     */
void QrCodeGenerator::qrCodePaint(QPainter *painter, const qrcodegen::QrCode &qrCode, quint16 border, const quint16 size) const
{
    qreal scale = qreal(size) / (qrCode.getSize()+2*border);
    painter->scale(scale,scale);
    painter->translate(border-0.02,border-0.02);

    for (int y = 0; y < qrCode.getSize(); y++)
    {
        for (int x = 0; x < qrCode.getSize(); x++)
        {
            if (qrCode.getModule(x, y))
            {
                QRectF rect(x, y, 1.04, 1.04);
                painter->drawRect(rect);
            }
        }
    }

}

/**
 * @brief Converts a QR code to a QImage.
 * @param qrCode The QR code to convert.
 * @param border The border size to use.
 * @param size The image size to generate.
 * @return QImage representing the QR code.
 */
QImage QrCodeGenerator::qrCodeToImage(const qrcodegen::QrCode &qrCode, quint16 border,
                                      quint16 size) const
{
    QImage image(size, size, QImage::Format_ARGB32);
    image.fill(Qt::white);
    QPainter painter(&image);

    QBrush brush(Qt::SolidPattern);
    brush.setColor(Qt::black);
    painter.setBrush(brush);
    painter.setPen(QPen(Qt::NoPen));

    //    painter.setRenderHint(QPainter::Antialiasing);

    qrCodePaint(&painter,qrCode,border,size);

    painter.end();

    return image;
}
