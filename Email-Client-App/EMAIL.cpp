#include "EMAIL.h"

EMAIL::EMAIL() {
	isRead = 0;
	keyMap = 0;
}

EMAIL::EMAIL(const std::vector<std::string>& buffer) {
	std::string temp = buffer[0].substr(buffer[0].find(':') + 2);
	recvTO = splitEmails(temp);
	temp = buffer[1].substr(buffer[1].find(':') + 2);
	recvCC = splitEmails(temp);
	sender = buffer[2].substr(buffer[2].find(':') + 2);
	subject = buffer[3].substr(buffer[3].find(':') + 2);
	int numLines = atoi(buffer[6].substr(buffer[6].find(':') + 2).c_str());
	for (int i = 8; i <= 8 + numLines - 1; i++) {
		body.push_back(buffer[i]);
	}
	isRead = false;
	attachFiles.resize(0);
	recvBCC.resize(0);
	keyMap = 0;
	//remove last null character
	sender.pop_back();
	if (recvTO.size()) recvTO[recvTO.size() - 1].pop_back();
	if (recvCC.size()) recvCC[recvCC.size() - 1].pop_back();
	if (recvBCC.size()) recvBCC[recvBCC.size() - 1].pop_back();
}

void EMAIL::show() {
	std::cout << "----------\n";
	std::cout << "To: ";
	for (const auto& x : recvTO) {
		std::cout << '<' << x << ">, ";
	}
	std::cout << "\b\n";

	std::cout << "CC: ";
	for (const auto& x : recvCC) {
		std::cout << '<' << x << ">, ";
	}
	std::cout << "\b\n";

	std::cout << "From: " << '<' << sender << '>' << '\n';
	std::cout << "Subject: " << subject << '\n';
	std::cout << "Content: " << '\n';
	std::cout << '\n';
	for (const auto& x : body) {
		std::cout << x << '\n';
	}
	std::cout << '\n';

	if (attachFiles.size() == 0) {
		std::cout << "No file attached.\n";
		std::cout << "----------\n";
	}
	else {
		int n = attachFiles.size();
		std::cout << n << " files attached.\n";
		for (int i = 0; i < n; i++) {
			std::cout << i + 1 << ". " << attachFiles[i].filePath << '\n';
		}
	}
}

void EMAIL::subShow(int i) {
	std::cout << i << ". ";
	if (isRead == false) std::cout << "<UNREAD> ";
	else std::cout << "         ";
	std::cout << "<" << sender << "> <" << subject << ">\n";
}

std::vector<std::string> splitEmails(const std::string& emailString) {
	std::istringstream iss(emailString);
	std::vector<std::string> emailAddresses;

	std::string email;
	while (std::getline(iss, email, ',')) {
		size_t start = email.find_first_not_of(" \t");
		size_t end = email.find_last_not_of(" \t");

		if (start != std::string::npos && end != std::string::npos) {
			emailAddresses.push_back(email.substr(start, end - start + 1));
		}
	}

	return emailAddresses;
}

void EMAIL::input(const std::string& local) {
	sender = local;
	std::cout << "This is the email content: (press ENTER to skip if not filled).\n";
	std::cout << "To: ";
	std::string temp;
	//std::cin.ignore();
	std::getline(std::cin, temp);
	recvTO = splitEmails(temp);
	std::cout << "CC: ";
	std::getline(std::cin, temp);
	recvCC = splitEmails(temp);
	std::cout << "BCC: ";
	std::getline(std::cin, temp);
	recvBCC = splitEmails(temp);
	std::cout << "Subject: ";
	std::getline(std::cin, subject);
	std::cout << "Body:\nEnter continuously, type a separate '.' to end.\n";
	while (std::getline(std::cin, temp)) {
		if (temp == ".") break;
		else body.push_back(temp);
	}
	std::cout << "Attach file? [1]yes [0]no : ";
	bool athF = false;
	std::cin >> athF;
	if (athF) {
		int num = 0;
		std::cout << "Number of files: ";
		std::cin >> num;
		std::cin.ignore();
		for (int i = 1; i <= num; i++) {
			std::cout << "Path of file " << i << ":\n";
			std::string pathAthF;
			std::getline(std::cin, pathAthF);
			if (pathAthF.find('.') == -1) {
				std::cout << "Invalid file path.\n";
				continue;
			}
			std::fstream isExist(pathAthF.c_str(), std::ios::in);
			if (!isExist) {
				std::cout << "Invalid file path.\n";
				isExist.close();
			}
			else {
				//limit size of attach file
				if (getFileSize(pathAthF) > 1048576) {//3MB
					std::cout << "Size limit exceeded.\n";
					isExist.close();
					continue;
				}
				std::string nameFile, extension;
				SplitPath(pathAthF, nameFile, extension);
				Attachment tmpAthF{ pathAthF, nameFile, getFileSize(pathAthF) };
				attachFiles.push_back(tmpAthF);
			}
			isExist.close();
		}
	}
}

void SplitPath(const std::string& filePath, std::string& fileName, std::string& fileExtension) {
	size_t lastSlash = filePath.find_last_of("/\\");
	size_t lastDot = filePath.find_last_of(".");
	if (lastDot != std::string::npos && lastDot > lastSlash) {
		fileName = filePath.substr((lastSlash != std::string::npos) ? lastSlash + 1 : 0, lastDot - ((lastSlash != std::string::npos) ? lastSlash + 1 : 0));
		fileExtension = filePath.substr(lastDot);
	}
	else {
		fileName = (lastSlash != std::string::npos) ? filePath.substr(lastSlash + 1) : filePath;
		fileExtension = filePath.substr(lastDot);
	}
}

std::streampos getFileSize(const std::string& filePath) {
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		return -1;
	}
	std::streampos fileSize = file.tellg();
	file.close();
	return fileSize;
}

bool EMAIL::inputF(const std::string& file) {
	std::fstream fileOpen(file.c_str(), std::ios::in);
	keyMap = atoi(file.substr(file.find("\\mail_") + 6, file.find("\\content.txt") - 5).c_str());
	if (fileOpen.is_open() == 0) {
		fileOpen.close();
		return false;
	}
	fileOpen >> isRead;
	fileOpen.ignore();
	std::getline(fileOpen, sender);
	std::string temp;
	std::getline(fileOpen, temp);
	std::stringstream ss{ temp };
	recvTO.resize(0);
	while (std::getline(ss, temp, '\t')) recvTO.push_back(temp);

	std::getline(fileOpen, temp);
	std::stringstream ss2{ temp };
	recvCC.resize(0);
	while (std::getline(ss, temp)) recvCC.push_back(temp);

	std::getline(fileOpen, temp);
	std::stringstream ss3{ temp };
	recvBCC.resize(0);
	while (std::getline(ss3, temp)) recvBCC.push_back(temp);

	std::getline(fileOpen, subject);
	int nLines = 0;
	fileOpen >> nLines;
	fileOpen.ignore();
	body.resize(0);
	for (int i = 0; i < nLines; i++) {
		std::getline(fileOpen, temp);
		body.push_back(temp);
	}
	attachFiles.resize(0);
	while (std::getline(fileOpen, temp)) {
		attachFiles.push_back(Attachment{ "N/A", temp });
	}
	fileOpen.close();
	return true;
}

void EMAIL::outputF(const std::string& file) {
	std::fstream fileOpen(file.c_str(), std::ios::out | std::ios::trunc);
	if (!fileOpen.is_open()) {
		std::cout << file << "\n";
		system("pause");
		fileOpen.close();
		return;
	}
	fileOpen << isRead << '\n';
	fileOpen << sender << '\n';
	for (const auto& x : recvTO) fileOpen << x << '\t';
	fileOpen << '\n';
	for (const auto& x : recvCC) fileOpen << x << '\t';
	fileOpen << '\n';
	for (const auto& x : recvBCC) fileOpen << x << '\t';
	fileOpen << '\n';
	fileOpen << subject << '\n';
	fileOpen << body.size() << '\n';
	for (const auto& x : body) fileOpen << x << '\n';
	for (const auto& x : attachFiles) fileOpen << x.fileName << '\n';
	fileOpen.close();
}


void extractNameAndNumber(const std::string& input, std::string& name, int& number) {
	std::istringstream iss(input);
	std::string token;
	std::getline(iss, token, '=');
	std::getline(iss, token, ',');
	size_t start = token.find_first_not_of(" ");
	size_t end = token.find_last_not_of(" ");
	name = token.substr(start, end - start + 1);
	std::getline(iss, token, ',');
	std::getline(iss, token);
	std::istringstream numberStream(token);
	numberStream >> number;
}