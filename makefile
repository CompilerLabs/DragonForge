all:
	# build embeded text files
	xxd -i ./Source/DragonForge/Standard/print.df > ./BuildTemps/DragonForgePrint.c
	xxd -i ./Source/DragonForge/Standard/cast.df > ./BuildTemps/DragonForgeCast.c
	xxd -i ./Source/DragonForge/Standard/buffer.df > ./BuildTemps/DragonForgeBuffer.c
	xxd -i ./Source/DragonForge/Standard/current.df > ./BuildTemps/DragonForgeCurrent.c
	xxd -i ./Source/DragonForge/Standard/list.df > ./BuildTemps/DragonForgeList.c
	xxd -i ./Source/DragonForge/Standard/context.df > ./BuildTemps/DragonForgeContext.c
	xxd -i ./Source/DragonForge/Standard/check.df > ./BuildTemps/DragonForgeCheck.c
	xxd -i ./Source/DragonForge/Standard/error.df > ./BuildTemps/DragonForgeError.c
	xxd -i ./Source/DragonForge/Standard/json.df > ./BuildTemps/DragonForgeJson.c
	xxd -i ./Source/DragonForge/Standard/time.df > ./BuildTemps/DragonForgeTime.c
	xxd -i ./Source/DragonForge/Standard/anvil.df > ./BuildTemps/DragonForgeAnvil.c
	xxd -i ./Source/DragonForge/Standard/compile.df > ./BuildTemps/DragonForgeCompile.c
	xxd -i ./Source/DragonForge/Standard/just_run.df > ./BuildTemps/DragonForgeJustRun.c

	# compile executables
	gcc ./Source/Main.c -Wall -Wextra -fsanitize=address -g -o ./../DragonForgeDebug.elf
	gcc ./Source/Main.c -o ./../DragonForge.elf
