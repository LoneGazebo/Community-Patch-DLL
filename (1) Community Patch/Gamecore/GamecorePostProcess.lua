function do_gamecore_post( core, vpreg )
	local string = string ;
	
	print("POSTPROCESS!") ;
	
	local iterators = { 
		__EVER_CREATED = 0;
		countActiveOrLost = function(tt) return tt.__EVER_CREATED - #tt end;
		tryPop = function(tt) local i = #tt if i > 0 then local val = tt[i]; tt[i] = nil; return val; end end;
		push = function(tt,val) tt[#tt+1] = val end;
		notifyNew = function(tt) tt.__EVER_CREATED = tt.__EVER_CREATED + 1 end;
	}; if debug then debug.iterators = iterators; end
	
	local function getSingleValueIterator( nextKeyValPairFunc )
		local initfunc = iterators:tryPop() ;
		if not initfunc then
			local nextfunc = nil ;
			local current_key = nil ;
			local closure = function()
				local current_val ;
				current_key, current_val = nextfunc(current_key) ;
				if current_key == nil then 
					current_val = nil
					nextfunc = nil
					iterators:push(initfunc) ; -- Put the iterator structure back into the pool
				end
				return current_val ;
			end
			initfunc = function( newNextKeyValPairFunc )
				nextfunc = newNextKeyValPairFunc ;
				return closure ;
			end
			iterators:notifyNew();
		end
		return initfunc( nextKeyValPairFunc ) ;
	end
	
	local closure14 = function( func, A,B,C,D ) local ret = (function( arg ) return func( arg, A,B,C,D) end); return ret; end;
	
	(function()
	
		local mttPlayer = vpreg.Player.__meta ;
		mttPlayer.__tostring = function(p) return string.format("Player#%u(%s)",p:GetID(),p:GetName()) end ;
		mttPlayer.__len = function(p) return p:GetID() end ;
		
		local Player = core.Player
		local Players = core.Players ;
		
		local IsAlive = Player.IsAlive ;
		local IsEverAlive = Player.IsEverAlive ;
		local IsMajor = Player.IsMajorCiv ;
		local IsMinor = Player.IsMinorCiv ;
		local IsBarb = Player.IsBarbarian ;
		
		local nextPlayerKeyVal = function( prev, major, minor, barb, dead )
			for it = (prev and prev+1 or 0), 63 do
				local pl = Players[it] ;
				if ((dead and IsEverAlive or IsAlive)(pl)) and ( 
					( major and IsMajor(pl) ) or
					( minor and IsMinor(pl) ) or
					( barb and IsBarb(pl) ) ) 
				then
					return it,pl;
				end
			end
		end
		
		local filters = { get = function(tab,A,B,C,D)
			local val = (A and 0 or 1) + (B and 0 or 2) + (C and 0 or 4) + (D and 0 or 8);
			local ret = tab[val] ;
			if not ret then ret = closure14(nextPlayerKeyVal,A,B,C,D); tab[val] = ret; end
			return ret;
		end;};
		
		local aliveMajorsItFilter = filters:get(true,false,false,false);
		local aliveMajorsIt = function() return getSingleValueIterator(aliveMajorsItFilter) end ;
		Players.Majors = aliveMajorsIt ;
		
		local aliveMinorsItFilter = filters:get(false,true,false,false);
		Players.Minors = function() return getSingleValueIterator(aliveMajorsItFilter) end ;
		
		Players.Filtered = function(includeMajor,includeMinors,includeBarbarians,allowDead)
			return getSingleValueIterator( filters:get(includeMajor,includeMinors,includeBarbarians,includeDead) );
		end;
		
		setmetatable(Players,{ __call = function(_,includeMinors,includeDead) 
			if not includeMinors and not includeDead then return aliveMajorsIt(); end
			return getSingleValueIterator( filters:get(true,includeMinors,false,includeDead) ) ;
		end;});
	end)();
end

