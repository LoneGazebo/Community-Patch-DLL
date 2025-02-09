--- @meta

-- CTD means Crash To Desktop

--- @class Locale
Locale = {}

--- @param textKey string
--- @param ... string | number
function Locale.Lookup(textKey, ...) end

--- Alias to `Locale.Lookup`
--- @see Locale.Lookup
--- @param textKey string
--- @param ... string | number
function Locale.ConvertTextKey(textKey, ...) end

--- ```lua
--- -- Example
--- Locale.LookupLanguage('en_US', 'TXT_KEY_TURNS') -- 'Turns'
--- ```
--- @param langType string
--- @param textKey string
--- @param ... string | number
--- @return string
function Locale.LookupLanguage(langType, textKey, ...) end

--- ```lua
--- -- Example
--- Locale.Compare('a', 'a') -- 0
--- Locale.Compare('a', 'abc') -- -1
--- Locale.Compare('abc', 'a') -- 1
--- ```
--- @param a string
--- @param b string
--- @return number
function Locale.Compare(a, b) end

--- @param str string
--- @return number
function Locale.Length(str) end

--- @param textKey string
--- @return boolean
function Locale.HasTextKey(textKey) end

--- @param str string
--- @param prefix string
--- @return boolean
function Locale.StartsWith(str, prefix) end

--- @param str string
--- @param suffix string
--- @return boolean
function Locale.EndsWith(str, suffix) end

--- Convert `str` to lower Case
--- @param str string
--- @return string
function Locale.ToLower(str) end

--- Convert `str` to UPPER Case
--- @param str string
--- @return string
function Locale.ToUpper(str) end

--- Truncates `str` up to `num` exclusive. If `ellipsis` is `true`, `num` must be `> 3`, otherwise throws error
--- ```lua
--- -- Example
--- Locale.TruncateString('123456', 4, true)     -- "..." (length 3)
--- Locale.TruncateString('123456', 4, false)    -- "123" (length 3)
--- Locale.TruncateString('123456789', 6, true)  -- "12..." (length 5)
--- Locale.TruncateString('123456789', 6, false) -- "12345" (length 5)
--- ```
--- @param str string
--- @param num number
--- @param ellipsis? boolean Should add ... before maxLengthExclusive (Ellipsis is included in length)
function Locale.TruncateString(str, num, ellipsis) end

--- Formats num  using specified format string
--- ```lua
--- -- Example
--- Locale.ToNumber(1234567890, "#,###,###,###") -- "1,234,567,890"
--- Locale.ToNumber(1234, "#,###,###,###") -- "1,234"
--- Locale.ToNumber(1234.555557, "#.###") --  "1234.556"
--- Locale.ToNumber(1234.555557, "#.#") -- "1234.6"
--- Locale.ToNumber(1234.555, '#') -- "12345"
--- Locale.ToNumber(1234.999, '#') -- "12345"
--- ```
--- @param num number
--- @param format string
function Locale.ToNumber(num, format) end

--- @class Language
--- @field Type string
--- @field DisplayName string

--- @class SpokenLanguage: Language
--- @field Path string

--- ```lua
--- -- Example
--- Locale.GetCurrentLanguage() -- { Type = 'en_US', DisplayName = 'English' }
--- ```
--- @return { Type: string, DisplayName: string }
function Locale.GetCurrentLanguage() end

--- ```lua
--- -- Example
--- Locale.GetCurrentSpokenLanguage() -- { Type = 'en_US', Path = 'English', DisplayName = 'English' }
--- ```
--- @return SpokenLanguage
function Locale.GetCurrentSpokenLanguage() end

--- @param langType string
--- @return boolean
function Locale.SetCurrentLanguage(langType) end

--- @param langType string
--- @return boolean
function Locale.SetCurrentSpokenLanguage(langType) end

--- ```lua
--- -- Example
--- Locale.GetSupportedLanguages() -- { [1] = { Type = 'en_US', DisplayName = 'English' } }
--- ```
--- @return Language[]
function Locale.GetSupportedLanguages() end

--- ```lua
--- -- Example
--- Locale.GetSupportedSpokenLanguages() -- { [1] = { Type = 'en_US', Path = 'English', DisplayName = 'English' } }
--- ```
--- @return SpokenLanguage[]
function Locale.GetSupportedSpokenLanguages() end

--- @param key    string  # The name of the hot key
--- @param ctrl?  boolean # Whether the control key is pressed.
--- @param alt?   boolean # Whether the alt key is pressed
--- @param shift? boolean # Whether the shift key is pressed
--- @return nil | string  # localized string for the hot key
function Locale.GetHotKeyDescription(key, ctrl, alt, shift) end

--- Checks if specified string is ASCII
--- <b> CTD on called with non-string argument </b>
--- @param str string
--- @return boolean
function Locale.IsASCII(str) end

--- Checks if specified string is UTF8
--- <b>  CTD on called with non-string argument </b>
--- @param str string
--- @return boolean
function Locale.IsUTF8(str) end

--- ```lua
--- -- Example
--- Locale.ConvertPosixTimeToString(0) -- "1970-01-01 01:00"
--- ```
--- @param time number
--- @return string
function Locale.ConvertPosixTimeToString(time) end

--- ```lua
--- -- Example
--- Locale.IsNilOrEmpty('') -- true
--- Locale.IsNilOrEmpty(nil) -- true
--- Locale.IsNilOrEmpty('  ') -- false
--- ```
--- @param str nil | string
--- @return boolean
function Locale.IsNilOrEmpty(str) end

--- Checks if `str` consists of whitespaces or is `nil`.
--- <b> Do not use this function to check strings like `"[NEWLINE]"`, `"[SPACE]"` etc. </b>
--- ```lua
--- -- Example
--- Locale.IsNilOrWhitespace('') -- true
--- Locale.IsNilOrWhitespace(nil) -- true
--- Locale.IsNilOrWhitespace('  ') -- true
--- Locale.IsNilOrWhitespace('\n\t\v') -- true
--- Locale.IsNilOrWhitespace('[NEWLINE]') -- false
--- ```
--- @param str nil | string
--- @return boolean
function Locale.IsNilOrWhitespace(str) end

--- ```lua
--- -- Example
--- Locale.Substring("1234567890", 1, 3) -- "123"
--- ```
--- @param str string
--- @param startIndex integer
--- @param endIndex integer
function Locale.Substring(str, startIndex, endIndex) end

--- Converts `num` to curreny string depending on current language.
--- ```lua
--- -- Example
--- -- if current language is en_US
--- Locale.ToCurrency(123) -- '$123.00'
--- Locale.ToCurrency(123.256) -- '$123.26'
--- -- if current language is pl_PL
--- Locale.ToCurrency(123) -- '123,00 zł'
--- ```
--- @param num number
--- @return string
function Locale.ToCurrency(num) end

--- Converts `num` to ordinal string depending on current language
--- ```lua
--- -- Example
--- -- if current language is en_US
--- Locale.ToOrdinal(1) -- "1ˢᵗ"
--- Locale.ToOrdinal(2) -- "2ⁿᵈ"
--- Locale.ToOrdinal(3) -- "3ʳᵈ"
--- Locale.ToOrdinal(12) -- "12ᵗʰ"
--- Locale.ToOrdinal(12.34) -- "12ᵗʰ"
--- -- if current language is pl_PL
--- Locale.ToOrdinal(1) -- "1."
--- ```
--- @param num number
--- @return string
function Locale.ToOrdinal(num) end

--- Convers `num` to percent, where `1` is `"100%"`. Expect different result depending on current language
--- ```lua
--- -- Example
--- -- if current language is en_US
--- Locale.ToPercent(1) -- "100%"
--- Locale.ToPercent(0.01) -- "1%"
--- Locale.ToPercent(555) -- "55,500%"
--- -- if current language is pl_PL
--- Locale.ToPercent(555) -- "55 500%"
--- Locale.ToPercent(1) -- "100%"
--- ```
--- @param num number
--- @return string
function Locale.ToPercent(num) end

--- Spells out specified `num` depending on current language
--- ```lua
--- -- Example
--- -- if current language is en_US
--- Locale.ToSpellout(0/0) -- CTD
--- Locale.ToSpellout(1) -- "one"
--- Locale.ToSpellout(-555.12) -- "minus five hundred fifty-five point one two"
--- Locale.ToSpellout(5/0) -- "eighteen quadrillion..." very long number, looks like something like max number
--- -- if current language is pl_PL
--- Locale.ToSpellout(555) -- "pięćset pięćdziesiąt pięć"
--- ```
--- @param num number
--- @return string
function Locale.ToSpellout(num) end

--- Converts specified `num` to roman numeral string.
--- It is limited by some maximum number after which
--- just converts number to current language representation
--- ```lua
--- -- Example
--- Locale.ToRomanNumeral(12) -- "XII"
--- Locale.ToRomanNumeral(3) -- "III"
--- Locale.ToRomanNumeral(10^7) -- "10,000,000"
--- ```
--- @param num number
--- @return string
function Locale.ToRomanNumeral(num) end

--- Unknown behaviour
--- @deprecated
--- @param ... unknown
function Locale.LocalSystemCPToUTF8(...) end

--- Unknown behaviour
--- @deprecated
--- @param ... unknown
function Locale.UTF8ToLocalSystemCP(...) end
