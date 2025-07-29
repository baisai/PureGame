cd PureCore
for /R %%f in (*.h *.hpp *.cpp *.cxx) do (
    clang-format -i %%f
)
cd ..\PureDb
for /R %%f in (*.h *.hpp *.cpp *.cxx) do (
    clang-format -i %%f
)
cd ..\PureEncrypt
for /R %%f in (*.h *.hpp *.cpp *.cxx) do (
    clang-format -i %%f
)
cd ..\PureLua
for /R %%f in (*.h *.hpp *.cpp *.cxx) do (
    clang-format -i %%f
)
cd ..\PureMsg
for /R %%f in (*.h *.hpp *.cpp *.cxx) do (
    clang-format -i %%f
)
cd ..\PureNet
for /R %%f in (*.h *.hpp *.cpp *.cxx) do (
    clang-format -i %%f
)
cd ..\PureApp
for /R %%f in (*.h *.hpp *.cpp *.cxx) do (
    clang-format -i %%f
)
cd ..\PureGame
for /R %%f in (*.h *.hpp *.cpp *.cxx) do (
    clang-format -i %%f
)
cd ..