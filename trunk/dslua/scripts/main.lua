-- set up screens for text output
SCREEN_BOTTOM   = 0
SCREEN_TOP   = 1
BGBotText = Screen.LoadTextBG()
Screen.Initialize( SCREEN_BOTTOM, BGBotText )

function listDirectory(path)
	directoryList = {}
	fileList = {}
	if(Directory.Open(path) == 1)then
		Directory.Reset()
		CurrentDirectory = Directory.Next()
		while(string.len(CurrentDirectory) > 0)do
			if(Directory.IsDir() == 0) then
				table.insert(fileList, CurrentDirectory)
			else
				table.insert(directoryList, CurrentDirectory)
			end
			CurrentDirectory = Directory.Next()
		end
		Directory.Close()
	end
	table.sort(directoryList, function(a,b) return string.lower(a)<string.lower(b) end)
	table.sort(fileList, function(a,b) return string.lower(a)<string.lower(b) end)
	return directoryList,fileList
end

currentDirectory = "/"

dirs, files = listDirectory(currentDirectory)
pos=0
lastpos=not pos
line=0
dirCount = 0
fileCount = 0

function changeDirectory(directory)
	currentDirectory = directory
	Directory.Chdir(currentDirectory .. "/")
	dirs, files = listDirectory(currentDirectory)
	pos=0
	lastpos=not pos
	line=0
	dirCount = 0
	fileCount = 0

	for i,v in ipairs(dirs) do
		dirCount = dirCount + 1
	end

	for i,v in ipairs(files) do
		fileCount = fileCount + 1
	end
end

function stringreverse(str)
    local reversed = "";
    for i = string.len(str), 1, -1 do
        reversed = reversed..string.sub(str,i,i);
    end
    return reversed;
end

function dofile (filename)
	local f = assert(loadfile(filename))
	return f()
end


firstTime = true

while(true)do
	if(firstTime)then
		firstTime = false
		changeDirectory("/")
	end
	if(lastpos == pos)then
	else
		lastpos=pos
		line = pos
		BGBotText:Clear()
		BGBotText:SetColor(6)
		for i,v in ipairs(dirs) do
			if(line >= 0 and line <= 19)then
				BGBotText:PrintXY(0,line," " .. v)
			end
			line=line+1
		end
		for i,v in ipairs(files) do
			if(string.lower(string.sub(v, string.len(v)-3)) == ".lua")then
				BGBotText:SetColor(2)
			else
				BGBotText:SetColor(0)
			end
			if(line >= 0 and line <= 23)then
				BGBotText:PrintXY(0,line," " .. v)
			end
			line=line+1 
		end
		BGBotText:PrintXY(0,0,">")
	end
	if(Pads.Down())then
		while(Pads.Down())do
		end
		if(line > 1)then
			pos = pos-1
		end
	end
	if(Pads.Up())then
		while(Pads.Up())do
		end
		if(pos < 0)then
			pos = pos+1
		end
	end
	aActive = false
	bActive = false
	if(Pads.A() or Pads.B())then
		while(Pads.A() or Pads.B())do
			if(Pads.A())then
				aActive = true
				bActive = false
			elseif(Pads.B())then
				bActive = true
				aActive = false
			end
		end
		if(aActive and (-pos < dirCount))then
			for i,v in pairs(dirs) do
				if(i==-pos+1)then
					if(v == ".")then
					elseif(v == "..")then
						currentDirectory = string.sub(currentDirectory, 1, string.len(currentDirectory) - string.find(stringreverse(currentDirectory), "/"))
						if(currentDirectory == "")then
							currentDirectory = "/"
						end
						changeDirectory(currentDirectory)
					else
						if(currentDirectory == "/")then
							currentDirectory = currentDirectory .. v
						else
							currentDirectory = currentDirectory .. "/" .. v
						end
						if(currentDirectory == "")then
							currentDirectory = "/"
						end
						changeDirectory(currentDirectory)
						break
					end
				end
			end
		elseif(bActive and (-pos < dirCount))then--fix this to ignore B in root directory
			currentDirectory = string.sub(currentDirectory, 1, string.len(currentDirectory) - string.find(stringreverse(currentDirectory), "/"))
			if(currentDirectory == "")then
				currentDirectory = "/"
			end
			changeDirectory(currentDirectory)
		elseif(aActive)then
			for i,v in pairs(files) do
				if(i==-pos-dirCount+1)then
					if(string.lower(string.sub(v, string.len(v)-3)) == ".lua")then
						Screen.ResetAll()
						dofile(v)
						Screen.ResetAll()
						BGBotText = Screen.LoadTextBG()
						Screen.Initialize( SCREEN_BOTTOM, BGBotText )
						changeDirectory(currentDirectory)
					end
				end
			end
		end
	end
end

-- wait until a key is pressed
DSLua.WaitForAnyKey()