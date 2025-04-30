#include <string>

bool getPendingLoadState();
void setPendingLoadState(std::string parentPath, std::string savePath);
void checkIfLoadStatePending();
