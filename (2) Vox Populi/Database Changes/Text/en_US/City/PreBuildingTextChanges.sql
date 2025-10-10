-- With the tooltip rework, most buildings don't need a Help text anymore
-- Preserving an empty text entry for modmod compatibility
UPDATE Language_en_US
SET Text = ''
WHERE Tag IN (SELECT Help FROM Buildings);
