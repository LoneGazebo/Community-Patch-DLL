--
-- DebugPrint
--

function dprint(...)
  if (bDebug) then
    print(string.format(...))
  end
end