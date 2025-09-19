local Color = CPK.Util.Color
local C = Color.FromType

local TextColor = {
	Reset = Color.New('ENDCOLOR', { -- Picked using color picker
		r = 255,
		g = 255,
		b = 200,
		a = 255,
	}),

	Empty = C('COLOR_EMPTY'),
	Clear = C('COLOR_CLEAR'),

	Red = C('COLOR_RED'),
	Blue = C('COLOR_BLUE'),
	Cyan = C('COLOR_CYAN'),
	Brown = C('COLOR_BROWN_TEXT'),
	Green = C('COLOR_GREEN'),
	White = C('COLOR_WHITE'),
	Yellow = C('COLOR_YELLOW'),
	Magenta = C('COLOR_MAGENTA'),

	Warning = C('COLOR_WARNING_TEXT'),
	Negative = C('COLOR_NEGATIVE_TEXT'),
	Positive = C('COLOR_POSITIVE_TEXT'),
	Highlight = C('COLOR_HIGHLIGHT_TEXT'),
	HighlightAlt = C('COLOR_ALT_HIGHLIGHT_TEXT'),
	HighlightAdv = C('COLOR_ADVISOR_HIGHLIGHT_TEXT'),

	Grey = C('COLOR_GREY'),
	GreyDark = C('COLOR_DARK_GREY'),
	GreyLight = C('COLOR_LIGHT_GREY'),
	GreyAlpha = C('COLOR_ALPHA_GREY'),

	Unit = C('COLOR_UNIT_TEXT'),
	Tech = C('COLOR_TECH_TEXT'),
	Food = C('COLOR_YIELD_FOOD'),
	Gold = C('COLOR_YIELD_GOLD'),
	Prod = C('COLOR_YIELD_PRODUCTION')
}

local T = TextColor

T.Faith = T.White
T.Belief = T.White

T.Policy = T.Magenta
T.Culture = T.Magenta

CPK.Text.Color = TextColor
