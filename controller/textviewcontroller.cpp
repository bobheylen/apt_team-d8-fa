#include "textviewcontroller.h"
#include <QTimer>

QLoggingCategory TextViewControllerCategory("textviewcontroller", QtDebugMsg);

void TextViewController::visualizeWorld(){
    auto myTiles = worldController->getTiles();
    auto myEnemies = worldController->getEnemies();
    auto myHealthpacks = worldController->getHealthpacks();
    auto protagonist = worldController->getProtagonist();

    // Determine the ASCII representation of different entities
    const QString horizontalBorder = "+---";
    const QString verticalEmptyTile = "|\u00A0\u00A0\u00A0";
    const QString verticalHealthPackTile = "| H ";
    const QString verticalEnemyTile = "| E ";
    const QString verticalProtagonistTile = "| P ";

    // Loop through each row
    for (int y = 0; y < worldController->getRows(); ++y) {
        // Add the horizontal border for each tile in the row
        for (int x = 0; x < worldController->getCols(); ++x) {
            asciiRepresentation += horizontalBorder;
        }
        asciiRepresentation += "+"; // Add last '+' of every row
        asciiRepresentation += QChar(0x2029); // Unicode for 'PARAGRAPH SEPARATOR' = '\n'

        // Loop through each column
        for (int x = 0; x < worldController->getCols(); ++x) {
            // Check if the current position contains an entity (health pack, enemy, protagonist)
            auto isHealthPack = std::find_if(myHealthpacks.begin(), myHealthpacks.end(),
                                             [x, y](const auto &hp) { return hp->getXPos() == x && hp->getYPos() == y; });
            auto isEnemy = std::find_if(myEnemies.begin(), myEnemies.end(),
                                        [x, y](const auto &enemy) { return enemy->getXPos() == x && enemy->getYPos() == y; });
            auto isProtagonist = (protagonist->getXPos() == x && protagonist->getYPos() == y);

            // Append the corresponding ASCII representation to the overall representation string
            if (isHealthPack != myHealthpacks.end()) {
                asciiRepresentation += verticalHealthPackTile;
            } else if (isEnemy != myEnemies.end()) {
                asciiRepresentation += verticalEnemyTile;
            } else if (isProtagonist) {
                asciiRepresentation += verticalProtagonistTile;
            } else {
                asciiRepresentation += verticalEmptyTile; // Spaces for empty tile
            }
        }
        asciiRepresentation += "|"; // Add last '|' of every row
        asciiRepresentation += QChar(0x2029); // Unicode for 'PARAGRAPH SEPARATOR' = '\n'
    }

    // Add the horizontal border after the last row
    for (int x = 0; x < worldController->getCols(); ++x) {
        asciiRepresentation += horizontalBorder;
    }
    asciiRepresentation += "+"; // Add last '+' of map

    // Create a widget to contain the text view
    textViewWidget = new QWidget;
    textLayout = new QVBoxLayout(textViewWidget);

    // Display the ASCII representation in a QTextEdit
    asciiTextEdit = new QTextEdit(asciiRepresentation);
    asciiTextEdit->setFont(QFont("Courier")); // Set a monospaced font for better alignment

    // Set line wrap mode to NoWrap
    //asciiTextEdit->setLineWrapMode(QTextEdit::NoWrap); // Deze lijn zorgt voor delay wanneer movement

    asciiTextEdit->setPlainText(asciiRepresentation);

    // Add the text view to the layout
    textLayout->addWidget(asciiTextEdit);

    // Use a QHBoxLayout to arrange the text box and button horizontally
    moveLayout = new QHBoxLayout;

    // Add the text box to the layout
    moveLineEdit = new QLineEdit;
    moveLineEdit->setPlaceholderText("Enter your command (e.g. left, right, up, down)");
    moveLayout->addWidget(moveLineEdit);

    // Create a button
    moveButton = new QPushButton("MOVE");
    connect(moveButton, &QPushButton::clicked, this, &TextViewController::handleMoveButtonClick);
    moveLayout->addWidget(moveButton);

    // Use a QHBoxLayout to arrange the text box and button horizontally
    navigateLayout = new QHBoxLayout;

    // Add the text box to the layout
    navigateLineEdit = new QLineEdit;
    navigateLineEdit->setPlaceholderText("Enter your coordinate (e.g. 3,8)");
    navigateLayout->addWidget(navigateLineEdit);

    // Create a button
    navigateButton = new QPushButton("NAVIGATE");
    connect(navigateButton, &QPushButton::clicked, this, &TextViewController::handleNavigateButtonClick);
    navigateLayout->addWidget(navigateButton);

    textLayout->addLayout(moveLayout);
    textLayout->addLayout(navigateLayout);
}

void TextViewController::handleMoveButtonClick(){
    // Handle the button click event
    // Retrieve text from the QLineEdit and store it in the member variable
    moveText = moveLineEdit->text();

    // Print the text in the TextBox for debugging
    qCDebug(TextViewControllerCategory) << moveText;

    // Check the stored text and perform actions accordingly
    auto protagonist = worldController->getProtagonist();
    int newX = protagonist->getXPos();
    int newY = protagonist->getYPos();

    // Flag to indicate whether the command is correct or not
    bool correctCommand = false;

    if (moveText.toLower() == "left") {
        qCDebug(TextViewControllerCategory) << "left action was triggered";
        newX = protagonist->getXPos() - 1;
        correctCommand = true;
    } else if (moveText.toLower() == "right") {
        qCDebug(TextViewControllerCategory) << "right action was triggered";
        newX = protagonist->getXPos() + 1;
        correctCommand = true;
    } else if (moveText.toLower() == "up") {
        qCDebug(TextViewControllerCategory) << "up action was triggered";
        newY = protagonist->getYPos() - 1;
        correctCommand = true;
    } else if (moveText.toLower() == "down") {
        qCDebug(TextViewControllerCategory) << "down action was triggered";
        newY = protagonist->getYPos() + 1;
        correctCommand = true;
    }

    // Check if the new position is within the boundaries of the world
    if(newX >= 0 && newX < worldController->getCols() && newY >= 0 && newY < worldController->getRows()) {
        // Update the protagonist's position only if it's a valid position
        protagonist->setXPos(newX);
        protagonist->setYPos(newY);

        // Redraw the protagonist and energy bar
        emit worldController->drawProtagonist();
        emit worldController->drawBars();

        // Check if you can attack an enemy or use a healthpack
        worldController->attackEnemy();
        worldController->useHealthpack();
    }

    // Change the background color of the button temporarily
    if (correctCommand) {
        // Green color for correct command
        moveLineEdit->setStyleSheet("background-color: lightgreen;");
    } else {
        // Red color for incorrect command
        moveLineEdit->setStyleSheet("background-color: lightcoral;");
    }

    // Set up a QTimer to revert the color after a short delay (e.g., 500 milliseconds)
    QTimer::singleShot(250, this, [this]() {
        // Restore the default background color
        moveLineEdit->setStyleSheet("");
        moveLineEdit->setText("");
    });
}

void TextViewController::handleNavigateButtonClick(){
    // Handle the button click event
    // Retrieve text from the QLineEdit and store it in the member variable
    navigateText = navigateLineEdit->text();

    // Print the text in the TextBox for debugging
    qCDebug(TextViewControllerCategory) << navigateText;

    // Check the stored text and perform actions accordingly
    auto protagonist = worldController->getProtagonist();
    int newX = protagonist->getXPos();
    int newY = protagonist->getYPos();

    if (navigateText.toLower() == "left") {
        qCDebug(TextViewControllerCategory) << "left action was triggered";
        newX = protagonist->getXPos() - 1;
    } else if (navigateText.toLower() == "right") {
        qCDebug(TextViewControllerCategory) << "right action was triggered";
        newX = protagonist->getXPos() + 1;
    } else if (navigateText.toLower() == "up") {
        qCDebug(TextViewControllerCategory) << "up action was triggered";
        newY = protagonist->getYPos() - 1;
    } else if (navigateText.toLower() == "down") {
        qCDebug(TextViewControllerCategory) << "down action was triggered";
        newY = protagonist->getYPos() + 1;
    }

    // Check if the new position is within the boundaries of the world
    if(newX >= 0 && newX < worldController->getCols() && newY >= 0 && newY < worldController->getRows()) {
        // Update the protagonist's position only if it's a valid position
        protagonist->setXPos(newX);
        protagonist->setYPos(newY);

        // Redraw the protagonist and energy bar
        emit worldController->drawProtagonist();
        emit worldController->drawBars();

        // Check if you can attack an enemy or use a healthpack
        worldController->attackEnemy();
        worldController->useHealthpack();
    }
}

void TextViewController::drawProtagonist() {
    // Update ASCII representation for Text view
    QString updatedAsciiRepresentation = asciiRepresentation;

    // Replace the old representation of the protagonist with an empty tile
    updatedAsciiRepresentation.replace(startProtagonistIndex, 1, "\u00A0");

    // Check what was under old position
    if (updatedAsciiRepresentation.at(oldProtagonistIndex) == 'E') {
        qCDebug(TextViewControllerCategory) << "previous was enemy";
        // TODO: handle E
    } else if (updatedAsciiRepresentation.at(oldProtagonistIndex) == 'H') {
        qCDebug(TextViewControllerCategory) << "previous was healthpack";
        // TODO: handle H
    }

    // Find the index corresponding to the new protagonist's position in the ASCII representation
    int newProtagonistIndex = worldController->getCols()*4 + 4*worldController->getProtagonist()->getXPos() + 2*worldController->getCols()*4*worldController->getProtagonist()->getYPos() + 4*worldController->getProtagonist()->getYPos()+4;

    // Replace the empty tile with the representation of the protagonist
    updatedAsciiRepresentation.replace(newProtagonistIndex, 1, "P");

    // Update oldIndex with newIndex
    oldProtagonistIndex = newProtagonistIndex;

    // Display the updated ASCII representation in the QTextEdit
    asciiTextEdit->setPlainText(updatedAsciiRepresentation);

}

void TextViewController::handleDeath(){
    //TODO
}

void TextViewController::handleHealthPackTaken(std::shared_ptr<Tile> pack){
    //TODO
}

void TextViewController::handlePoisonLevelUpdated(float poisonLevel) {

}
