print("loading SquadsUtil.lua")

function ValidateText(text)

    local numNonWhiteSpace = 0;
    for i = 1, #text, 1 do
        if string.byte(text, i) ~= 32 then
            numNonWhiteSpace = numNonWhiteSpace + 1;
        end
    end
    
    if numNonWhiteSpace < 3 then
        return false;
    end
    
    -- don't allow % character
    for i = 1, #text, 1 do
        if string.byte(text, i) == 37 then
            return false;
        end
    end
    
    local invalidCharArray = { '\"', '<', '>', '|', '\b', '\0', '\t', '\n', '/', '\\', '*', '?', '%[', ']' };

    for i, ch in ipairs(invalidCharArray) do
        if string.find(text, ch) ~= nil then
            return false;
        end
    end
    
    -- don't allow control characters
    for i = 1, #text, 1 do
        if string.byte(text, i) < 32 then
            return false;
        end
    end
    
    return true;
end










print("finished loading SquadsUtil.lua")