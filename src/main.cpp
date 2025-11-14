#include "application.hpp"

int main()
{
    FileContactRepository repository("contacts.txt");
    ConsoleApplication app(repository);
    app.run();
    return 0;
}
