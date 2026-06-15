import "io";

print("IO Extension Test");

print("Enter your name:");
let name = input();
print("Hello, " + name + "!");

writeFile("test.txt", "ExLang IO Test File\nHello from ExLang!");
print("File written: test.txt");

let content = readFile("test.txt");
print("File content:");
print(content);
