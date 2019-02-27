function GetLinkFiles()
  local linkFiles = os.matchfiles("libs/**.lib")
  local directories = {}
  local names = {}

  for _, file in ipairs(linkFiles) do
    table.insert(directories, path.getdirectory(file))
    table.insert(names, path.getbasename(file))
  end

  return directories, names
end

function SetConfiguration(projectName, config)
  local configName = { config }

  configuration(configName)
    characterset("Unicode")
    symbols("On")
    warnings("Extra")
    postbuildcommands({})

    targetdir("solution/output/" .. projectName .."/")

    defineList = { "DEBUG_MODE=0", "BETA_MODE=1", "RELEASE_MODE=2", "UNICODE" }
    exeName = projectName

    local suffix = ""

    if config == "Debug" then
      suffix = suffix .. "_Debug"
      warnings("Extra")
      optimize("Off")
      defineList[#defineList + 1] = "MODE=DEBUG_MODE"
    elseif config == "Beta" then
      suffix = suffix .. "_Beta"
      warnings("Extra")
      optimize("Full")
      defineList[#defineList + 1] = "MODE=BETA_MODE"
    elseif config == "Release" then
      optimize("Full")
      defineList[#defineList + 1] = "MODE=RELEASE_MODE"
    end

    defineList[#defineList + 1] =
      "DEBUG_ENABLED=(MODE == DEBUG_MODE || MODE == BETA_MODE)"

    exeName = exeName .. suffix

    targetname(exeName)
    defines(defineList)
end

function SetConfigurations(projectName)
  SetConfiguration(projectName, "Debug")
  SetConfiguration(projectName, "Beta")
  SetConfiguration(projectName, "Release")
end

function BuildProject(projectName, lib, link, dlls)
  local projectDir = "projects/" .. projectName .. "/"

  local includes = ""
  local includeDir = os.matchdirs("include/" .. projectDir)

  if #includeDir ~= 0 then
    includes = "include/" .. projectDir .. "**"
  end

  local sources = "source/" .. projectDir .. "**"

  project(projectName)
    location("solution/projects/" .. projectName)
    language("C++")
    kind("ConsoleApp")
    includedirs({"include/" .. projectDir, "include/engine/", "include/"})
    libdirs(lib)
    links(link)

    files({ "include/engine/**", includes, sources, "resource/**" })

    SetConfigurations(projectName)

  local outputFile = "solution/output/" .. projectName .."/"

  for _, file in ipairs(dlls) do
    os.copyfile(file, "solution/" .. projectDir .. path.getname(file))
    os.copyfile(file, outputFile .. path.getname(file))
  end
end

function BuildEngine(lib, link)

  project("Engine")
    location("solution/engine/")
    language("C++")
    kind("StaticLib")
    includedirs({"include/engine/", "include/"})
    libdirs(lib)
    links(link)

    files({ "include/engine/**", "include/external/**", "source/engine/**"})

    SetConfigurations("Engine")

  table.insert(lib, "solution/output/engine")
  table.insert(link, "Engine")
end

function BuildProjects(lib, link, dlls)
  local projects = os.matchdirs("source/projects/*")

  if #projects == 0 then
    return
  end

  for _, dir in ipairs(projects) do
    projectName = path.getbasename(dir)
    BuildProject(projectName, lib, link, dlls)
  end
end

function AddSubFoldersIfExist(subFolderList, baseFolderName)
  local baseFolders = os.matchdirs(baseFolderName)

  if #baseFolders ~= 0 then
    subFolderList[#subFolderList + 1] = baseFolderName .. "**"
  end
end

function BuildTest(projectName, lib, link)

  local testDir = "test/" .. projectName .. "/"
  local projectDir = "projects/" .. projectName .. "/"

  local includes = ""
  local includeDir = os.matchdirs("include/" .. projectDir)

  local filesToInclude =
  {
    "include/engine/**",
    "resource/**"
  }

  AddSubFoldersIfExist(filesToInclude, "include/" .. projectDir)
  AddSubFoldersIfExist(filesToInclude, "include/" .. testDir)
  AddSubFoldersIfExist(filesToInclude, "source/" .. projectDir)
  AddSubFoldersIfExist(filesToInclude, "source/" .. testDir)

  if #filesToInclude == 0 then
    print("failure adding \"Test" .. projectName .. "\"")
    return
  end

  local testName = "Test" .. projectName

  project(testName)
    location("solution/tests/" .. projectName)
    language("C++")
    kind("ConsoleApp")
    includedirs({"include/" .. projectDir, "include/" .. testDir, "include/"})
    libdirs(lib)
    links(link)

    files(filesToInclude)
    excludes("source/" .. projectDir .. "Main.cpp")

    SetConfigurations(testName)
end

function BuildTests(lib, link)
  local tests = os.matchdirs("source/test/*")

  if #tests == 0 then
    return
  end

  for _, dir in ipairs(tests) do
    projectName = path.getbasename(dir)
    BuildTest(projectName, lib, link)
  end
  
end

function MoveAssetsForProject(projectName)

  local assets = os.matchfiles("assets/" .. projectName .. "/**")

  if #assets ~= 0 then
    projectAssetDir = "solution/projects/" .. projectName .. "/assets/"
    ouputAssetDir = "solution/output/" .. projectName .. "/assets/"

    os.mkdir(projectAssetDir)
    os.mkdir(ouputAssetDir)

    for _, file in ipairs(assets) do
      os.copyfile(file, projectAssetDir .. path.getname(file))
      os.copyfile(file, ouputAssetDir .. path.getname(file))
    end
  end
end

function MoveAssets()
  local projects = os.matchdirs("source/projects/*")

  if #projects == 0 then
    return
  end

  for _, dir in ipairs(projects) do
    projectName = path.getbasename(dir)
    MoveAssetsForProject(projectName)
  end
end

function BuildAll()
  local name = path.getname(path.getabsolute("./"))
  local lib, link = GetLinkFiles()
  local dlls = os.matchfiles("dlls/**.dll")

  os.mkdir("solution/output/")

  solution(name)
    location("solution/")
    objdir("solution/intermediate/")
    configurations({ "Debug", "Beta", "Release" })
    platforms({ "x64" })
    buildoptions({"/std:c++latest"})

    BuildEngine(lib, link)
    BuildProjects(lib, link, dlls)
    BuildTests(lib, link)

  MoveAssets()
end

BuildAll()
