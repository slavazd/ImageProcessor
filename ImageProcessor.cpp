#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <regex>
#include <Magick++.h>

using namespace std;

static void help()
{	
	cout << "Type load or ld <name> <filename> for loading image\n";
	cout << "Type store or s <name> <filename> for saving image\n";
	cout << "Type blur <from_name> <to_name> <size> for bluring image\n";
	cout << "Type resize <from_name> <filename> <new_width> <new_height> for resizing image\n";
	cout << "Type help or h for reading help\n";
	cout << "Type exit,quit or q for exit\n";
}

bool is_number(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](unsigned char c) { return !isdigit(c); }) == s.end();
}

vector<string> split(const string& input, const regex re)
{
	vector<string> tokens;
	for (auto it = sregex_iterator(input.cbegin(), input.cend(), re); it != sregex_iterator(); ++it)
		tokens.push_back(it->str());

	return tokens;
}

void chop(string str)
{
	if ((str[0] == '\"') && (str[str.length() - 1] == '\"'))
	{
		str.pop_back();
		str.erase(0, 1);
	}
}

int parseLoadString(const string &command,map<string, Magick::Image*>& imagesMap)
{
	const regex re{ "\".+?\"|[^ ^\t]+" };
	vector<string> tokens = split(command, re);
	if (tokens.size() != 3)
		return -1;
	if (tokens[1].find("\"") != string::npos)
		return -1;
	chop(tokens[2]);
	Magick::Image *image = new Magick::Image(tokens[2]);
	if (image == 0)
		return -2;
	imagesMap[tokens[1]] = image;
	return 0;
}
int parseSaveString(const string &command,map<string, Magick::Image*>& imagesMap)
{
	const regex re{ "\".+?\"|[^ ^\t]+" };
	vector<string> tokens = split(command, re);
	if (tokens.size() != 3)
		return -1;
	if (tokens[1].find("\"") != string::npos)
		return -1;
	chop(tokens[2]);
	Magick::Image* image = 0;
	if (imagesMap.find(tokens[1]) != imagesMap.end())
		image = imagesMap[tokens[1]];
	if (image == 0)
		return -2;
	image->write(tokens[2]);
	return 0;
}
int parseBlurString(const string &command,map<string, Magick::Image*>& imagesMap)
{
	const regex re{ "[0-9a-zA-Z_]+" };
	vector<string> tokens = split(command, re);
	if (tokens.size() != 4)
		return -1;
	if (tokens[0].compare("blur") != 0)
		return -1;
	Magick::Image* image = 0;
	if (imagesMap.find(tokens[1]) != imagesMap.end())
		image = imagesMap[tokens[1]];
	if (image == 0)
		return -2;
	Magick::Image* blur_image = new Magick::Image(*image);
	if(!is_number(tokens[3]))
		return -3;
	blur_image->blur(stoi(tokens[3]));
	imagesMap[tokens[2]] = blur_image;
	
	return 0;
}
int parseResizeString(const string &command,map<string, Magick::Image*>& imagesMap)
{
	const regex re{ "[0-9a-zA-Z_]+" };
	vector<string> tokens = split(command, re);
	if (tokens.size() != 5)
		return -1;
	if (tokens[0].compare("resize") != 0)
		return -1;
	Magick::Image* image = 0;
	if (imagesMap.find(tokens[1]) != imagesMap.end())
		image = imagesMap[tokens[1]];
	if (image == 0)
		return -2;
	Magick::Image* resize_image = new Magick::Image(*image);
	if (!is_number(tokens[3]) || !is_number(tokens[4]))
		return -3;
	resize_image->resize(Magick::Geometry(stoi(tokens[3]),stoi(tokens[4])));
	imagesMap[tokens[2]] = resize_image;

	return 0;
}


int main(int argc, char **argv)
{
	string command,command_line;
	bool quit = true;
	map<string, Magick::Image*> loadedImagesMap;
	help();
	do {
		getline(cin,command_line);

		const regex re{"\".+?\"|[^ ^\t]+"};
		vector<string> tokens = split(command_line, re);
		if (tokens.size() >= 1)
			command = tokens[0];
		else
		{
			cout << "incorrect command line\n";
			continue;
		}

		if ( (command.compare("q") == 0) || (command.compare("quit") == 0) || (command.compare("exit") == 0))
		{
			for (auto it = loadedImagesMap.begin(); it != loadedImagesMap.end(); ++it)
				delete it->second;
			quit = false;
			break;
		}
		if ((command.compare("help") == 0) || (command.compare("h") == 0))
		{
			help();
			continue;
		}
		if ((command.compare("load") == 0) || (command.compare("ld") == 0))
		{
			
			if ((tokens.size() >= 3) && (tokens[2].find(" ") != string::npos))
			{
				cout << "Sorry, ImageMagick doesn't support file path with spaces inside\n";
				continue;
			}
			
			if(parseLoadString(command_line, loadedImagesMap))
				cout << "incorrect command\n";
			continue;
		}
		if ((command.compare("store") == 0) || (command.compare("s") == 0))
		{
			if ((tokens.size() >= 3) && (tokens[2].find(" ") != string::npos))
			{
				cout << "Sorry, ImageMagick doesn't support file path with spaces inside\n";
				continue;
			}
			if(parseSaveString(command_line, loadedImagesMap))
				cout << "incorrect command\n";
			continue;
		}
		if ((command.compare("blur") == 0) )
		{
			if(parseBlurString(command_line, loadedImagesMap))
				cout << "incorrect command\n";
			continue;
		}
		if ((command.compare("resize") == 0))
		{
			if(parseResizeString(command_line, loadedImagesMap))
				cout << "incorrect command\n";
			continue;
		}
		cout << "incorrect command\n";
	} while (quit);
}

