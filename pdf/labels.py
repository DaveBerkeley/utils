#!/usr/bin/python
# -*- coding: latin-1 -*-

#
#   Printing Address Labels using reportlab
#

import sys

# Generate PDFs using ReportLab
from reportlab.lib.units import cm
from reportlab.pdfgen import canvas
from reportlab.lib.pagesizes import A4
from reportlab.graphics.barcode import code39

#
#
 
def make_label(canvas, lines, bar_data, size):
    x_offset = size[0] / 15.0
    y_offset = size[1] / 20.0

    address = { "fontName" : "Helvetica", "fontSize" : 12 }
    small = { "fontName" : "Helvetica", "fontSize" : 8 }

    x = x_offset
    y = y_offset
    dy = 20

    style = address
    for text in lines:
        canvas.setFont(style["fontName"], style["fontSize"])
        canvas.drawString(x, size[1] - y, text)
        y += dy

    if not bar_data:
        return

    # Generate the unique barcode
    bar_width = 0.04 
    bar_height = 0.3 
    barcode = code39.Extended39(bar_data, barWidth=bar_width * cm, barHeight= bar_height * cm, checksum=0, bearers=0.5, quiet=0)
    barcode.drawOn(canvas, x, size[1] - y)

    tails = (
        bar_data,
    )

    y += 10
    style = small
    for text in tails:
        canvas.setFont(style["fontName"], style["fontSize"])
        canvas.drawString(x, size[1] - y, text)
        y += dy

#
#
 
def main():
    size = A4
    c = canvas.Canvas("labels.pdf", pagesize=size)

    road_group = "RG 14"

    people = (
        (
            "Raffles Winner",
            "Lockyer Hall",
            "Alfred Street",
            "Plymouth ",
            "PL1 2RP",
        ),
        (
            "Lance Boyle",
            "10 Downing Street",
            "Westminster",
            "London",
            "SW1A 2AA",
        ),
    )
 
    for lines in people:
        make_label(c, lines, road_group, size)
        c.showPage()

    c.save()

#
#

if __name__ == "__main__":
    main()

# FIN
