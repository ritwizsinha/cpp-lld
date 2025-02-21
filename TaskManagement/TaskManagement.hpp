#include <string>
#include <vector>

enum class TaskStatus {
    PENDING,
    IN_PROGRESS,
    COMPLETED
};

class Task {
    std::string title;
    std::string description;
    int dueDate;
    int priority;
    TaskStatus status;
};

class User {
    public:
    int id;
    std::string name;
    std::vector<Task> active
}