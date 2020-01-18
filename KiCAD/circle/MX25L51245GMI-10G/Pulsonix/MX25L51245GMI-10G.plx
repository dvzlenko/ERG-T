PULSONIX_LIBRARY_ASCII "SamacSys ECAD Model"
//539862/78564/2.35/16/4/Transistor

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r187_65"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 0.65) (shapeHeight 1.87))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(textStyleDef "Normal"
		(font
			(fontType Stroke)
			(fontFace "Helvetica")
			(fontHeight 1.27)
			(strokeWidth 0.127)
		)
	)
	(patternDef "SOIC127P1030X265-16N" (originalName "SOIC127P1030X265-16N")
		(multiLayer
			(pad (padNum 1) (padStyleRef r187_65) (pt -4.69, 4.445) (rotation 90))
			(pad (padNum 2) (padStyleRef r187_65) (pt -4.69, 3.175) (rotation 90))
			(pad (padNum 3) (padStyleRef r187_65) (pt -4.69, 1.905) (rotation 90))
			(pad (padNum 4) (padStyleRef r187_65) (pt -4.69, 0.635) (rotation 90))
			(pad (padNum 5) (padStyleRef r187_65) (pt -4.69, -0.635) (rotation 90))
			(pad (padNum 6) (padStyleRef r187_65) (pt -4.69, -1.905) (rotation 90))
			(pad (padNum 7) (padStyleRef r187_65) (pt -4.69, -3.175) (rotation 90))
			(pad (padNum 8) (padStyleRef r187_65) (pt -4.69, -4.445) (rotation 90))
			(pad (padNum 9) (padStyleRef r187_65) (pt 4.69, -4.445) (rotation 90))
			(pad (padNum 10) (padStyleRef r187_65) (pt 4.69, -3.175) (rotation 90))
			(pad (padNum 11) (padStyleRef r187_65) (pt 4.69, -1.905) (rotation 90))
			(pad (padNum 12) (padStyleRef r187_65) (pt 4.69, -0.635) (rotation 90))
			(pad (padNum 13) (padStyleRef r187_65) (pt 4.69, 0.635) (rotation 90))
			(pad (padNum 14) (padStyleRef r187_65) (pt 4.69, 1.905) (rotation 90))
			(pad (padNum 15) (padStyleRef r187_65) (pt 4.69, 3.175) (rotation 90))
			(pad (padNum 16) (padStyleRef r187_65) (pt 4.69, 4.445) (rotation 90))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0, 0) (textStyleRef "Normal") (isVisible True))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt -5.875 5.5) (pt 5.875 5.5) (width 0.05))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt 5.875 5.5) (pt 5.875 -5.5) (width 0.05))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt 5.875 -5.5) (pt -5.875 -5.5) (width 0.05))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt -5.875 -5.5) (pt -5.875 5.5) (width 0.05))
		)
		(layerContents (layerNumRef 28)
			(line (pt -3.755 5.15) (pt 3.755 5.15) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt 3.755 5.15) (pt 3.755 -5.15) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt 3.755 -5.15) (pt -3.755 -5.15) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt -3.755 -5.15) (pt -3.755 5.15) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt -3.755 3.88) (pt -2.485 5.15) (width 0.025))
		)
		(layerContents (layerNumRef 18)
			(line (pt -3.405 5.15) (pt 3.405 5.15) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 3.405 5.15) (pt 3.405 -5.15) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 3.405 -5.15) (pt -3.405 -5.15) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -3.405 -5.15) (pt -3.405 5.15) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -5.625 5.12) (pt -3.755 5.12) (width 0.2))
		)
	)
	(symbolDef "MX25L51245GMI-10G" (originalName "MX25L51245GMI-10G")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -25 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 2) (pt 0 mils -100 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -125 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 3) (pt 0 mils -200 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -225 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 4) (pt 0 mils -300 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -325 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 5) (pt 0 mils -400 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -425 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 6) (pt 0 mils -500 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -525 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 7) (pt 0 mils -600 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -625 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 8) (pt 0 mils -700 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -725 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 9) (pt 1500 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1270 mils -25 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 10) (pt 1500 mils -100 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1270 mils -125 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 11) (pt 1500 mils -200 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1270 mils -225 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 12) (pt 1500 mils -300 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1270 mils -325 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 13) (pt 1500 mils -400 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1270 mils -425 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 14) (pt 1500 mils -500 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1270 mils -525 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 15) (pt 1500 mils -600 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1270 mils -625 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 16) (pt 1500 mils -700 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1270 mils -725 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(line (pt 200 mils 100 mils) (pt 1300 mils 100 mils) (width 6 mils))
		(line (pt 1300 mils 100 mils) (pt 1300 mils -800 mils) (width 6 mils))
		(line (pt 1300 mils -800 mils) (pt 200 mils -800 mils) (width 6 mils))
		(line (pt 200 mils -800 mils) (pt 200 mils 100 mils) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 1350 mils 300 mils) (justify Left) (isVisible True) (textStyleRef "Normal"))
		(attr "Type" "Type" (pt 1350 mils 200 mils) (justify Left) (isVisible True) (textStyleRef "Normal"))

	)
	(compDef "MX25L51245GMI-10G" (originalName "MX25L51245GMI-10G") (compHeader (numPins 16) (numParts 1) (refDesPrefix Q)
		)
		(compPin "1" (pinName "NC/SIO3") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "2" (pinName "VCC") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "3" (pinName "RESET#") (partNum 1) (symPinNum 3) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "4" (pinName "NC_1") (partNum 1) (symPinNum 4) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "5" (pinName "DNU_1") (partNum 1) (symPinNum 5) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "6" (pinName "DNU_2") (partNum 1) (symPinNum 6) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "7" (pinName "CS#") (partNum 1) (symPinNum 7) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "8" (pinName "SO/SIO1") (partNum 1) (symPinNum 8) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "16" (pinName "SCLK") (partNum 1) (symPinNum 9) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "15" (pinName "SI/SIO0") (partNum 1) (symPinNum 10) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "14" (pinName "NC_3") (partNum 1) (symPinNum 11) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "13" (pinName "NC_2") (partNum 1) (symPinNum 12) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "12" (pinName "DNU_4") (partNum 1) (symPinNum 13) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "11" (pinName "DNU_3") (partNum 1) (symPinNum 14) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "10" (pinName "GND") (partNum 1) (symPinNum 15) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "9" (pinName "WP#/SIO2") (partNum 1) (symPinNum 16) (gateEq 0) (pinEq 0) (pinType Unknown))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "MX25L51245GMI-10G"))
		(attachedPattern (patternNum 1) (patternName "SOIC127P1030X265-16N")
			(numPads 16)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
				(padNum 3) (compPinRef "3")
				(padNum 4) (compPinRef "4")
				(padNum 5) (compPinRef "5")
				(padNum 6) (compPinRef "6")
				(padNum 7) (compPinRef "7")
				(padNum 8) (compPinRef "8")
				(padNum 9) (compPinRef "9")
				(padNum 10) (compPinRef "10")
				(padNum 11) (compPinRef "11")
				(padNum 12) (compPinRef "12")
				(padNum 13) (compPinRef "13")
				(padNum 14) (compPinRef "14")
				(padNum 15) (compPinRef "15")
				(padNum 16) (compPinRef "16")
			)
		)
		(attr "Manufacturer_Name" "Macronix")
		(attr "Manufacturer_Part_Number" "MX25L51245GMI-10G")
		(attr "RS Part Number" "9140674")
		(attr "RS Price/Stock" "http://uk.rs-online.com/web/p/products/9140674")
		(attr "Arrow Part Number" "MX25L51245GMI-10G")
		(attr "Arrow Price/Stock" "https://www.arrow.com/en/products/mx25l51245gmi-10g/macronix-international")
		(attr "Description" "MX25L51245GMI-10G, Serial NOR Flash Memory, 128M x 4 bit, 256M x 2 bit, 512M x 1 bit 512Mbit, 2.7  3.6 V")
		(attr "<Hyperlink>" "http://docs-asia.electrocomponents.com/webdocs/14ac/0900766b814ac719.pdf")
		(attr "<Component Height>" "2.65")
		(attr "<STEP Filename>" "MX25L51245GMI-10G.stp")
	)

)
