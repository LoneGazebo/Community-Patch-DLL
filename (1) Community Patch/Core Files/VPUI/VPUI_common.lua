print("Loading VPUI_common");

-- Display active DLC and mods
print("========== Active Content ==========");

-- Display DLC packages
for _, v in pairs(ContentManager.GetAllPackageIDs()) do
	print(ContentManager.IsActive(v, ContentType.GAMEPLAY) and "Active DLC:" or "Disabled:  ", v, Locale.Lookup(ContentManager.GetPackageDescription(v)));
end

-- Display active mods
for _, v in pairs(Modding.GetActivatedMods()) do
	print("Active MOD:", v.ID, Modding.GetModProperty(v.ID, v.Version, "Name"), "Version", v.Version);
end

print("====================================");
