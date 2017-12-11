from subprocess import call
call(["make"])
for i in range(1, 31):
	open("./kek/kek" + str(i) + ".bmp", "w")
	call(["./build/bin/align", "./pics1/" + str(i) + ".bmp", "./kek/kek" + str(i) + ".bmp", "--align", "--unsharp"])