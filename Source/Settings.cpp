#include "Settings.h"
#include "ui_Settings.h"
#include "DRMSetupWizard.h"

/** Settings constructor
* Initialize the settings UI
* \param p Inherited palette configuration for setting StyleSheets.
* \param parent Pointer to parent widget.
*/
Settings::Settings(QSettings* p, QWidget* parent) : QWidget(parent), ui(new Ui::Settings)
{
	ui->setupUi(this);
	this->setObjectName("settingsUI");
	this->setStyleSheet("background-color: " + p->value("Primary/SecondaryBase").toString() + ";} "
        "QLabel { color:  #ffffff;}"
        "QPushButton {"
        "color: " + p->value("Primary/LightText").toString() + "; "
        "background-color: " + p->value("Primary/DarkElement").toString() + "; "
        "border: none; margin: 0px; padding: 0px;} "
        "QPushButton:hover {"
        "background-color: " + p->value("Primary/InactiveSelection").toString() + ";} "
        "color: " + p->value("Primary/LightText").toString() + ";");

    QFont buttonFont("SourceSansPro", 9);

    ui->WizardButton->setFont(buttonFont);
    ui->WizardButton->setText("Add Games to Horizon");

    ui->ClearDatabaseButton->setFont(buttonFont);
    ui->ClearDatabaseButton->setText("Clear Database");

    ui->BodyColor->setFont(buttonFont);
    ui->BodyColor->setStyleSheet("background-color: " + p->value("Body/Background").toString() + ";}");

    ui->NavbarBG->setFont(buttonFont);
    ui->NavbarBG->setStyleSheet("background-color: " + p->value("Navbar/Background").toString() + ";}");

    ui->NavbarHover->setFont(buttonFont);
    ui->NavbarHover->setStyleSheet("background-color: " + p->value("Navbar/HoverColor").toString() + ";}");

    ui->NavbarSelected->setFont(buttonFont);
    ui->NavbarSelected->setStyleSheet("background-color: " + p->value("Navbar/SelectedColor").toString() + ";}");

	ui->TitleBarColor->setFont(buttonFont);
	ui->TitleBarColor->setStyleSheet("background-color: " + p->value("TitleBar/Color").toString() + ";}");

    ui->ResetColors->setFont(buttonFont);

    ui->label_2->setStyleSheet("{color: #FFFFFF}");
    ui->UserSettingsBox->setStyleSheet("color: #FFFFFF;} ");
    ui->ClientSettingsBox->setStyleSheet("color: #FFFFFF;} ");
    ui->StyleSettingsBox->setStyleSheet("color: #FFFFFF;} ");

	connect(ui->WizardButton, SIGNAL(clicked()), QApplication::instance(), SLOT(Settings::on_WizardButton_clicked()));
    connect(ui->BodyColor, SIGNAL(clicked()), QApplication::instance(), SLOT(Settings::on_BodyColor_clicked()));
    connect(ui->NavbarBG, SIGNAL(clicked()), QApplication::instance(), SLOT(Settings::on_NavbarBG_clicked()));
    connect(ui->NavbarHover, SIGNAL(clicked()), QApplication::instance(), SLOT(Settings::on_NavbarHover_clicked()));
    connect(ui->NavbarSelected, SIGNAL(clicked()), QApplication::instance(), SLOT(Settings::on_NavbarSelected_clicked()));
    connect(ui->TitleBarColor, SIGNAL(clicked()), QApplication::instance(), SLOT(Settings::on_TitleBarColor_clicked()));
    connect(ui->ResetColors, SIGNAL(clicked()), QApplication::instance(), SLOT(Settings::on_ResetColors_clicked()));
    connect(ui->ClearDatabaseButton, SIGNAL(clicked()), QApplication::instance(), SLOT(Settings::on_ClearDatabaseButton_clicked()));

    if (!db.init())
    {
        QMessageBox error;
        error.critical(0, "Error!", "An error occured while trying to load the database.");
        exit(EXIT_FAILURE);
    }
}

/** Event handler for Wizard Button
* Runs setup wizard on click
*/
void Settings::on_WizardButton_clicked()
{
    DRMSetupWizard* wiz = new DRMSetupWizard();
    wiz->show();
}

void Settings::on_BodyColor_clicked()
{
    QColor color = QColorDialog::getColor(Qt::white);
    updateColor(1, color);
}

void Settings::on_NavbarBG_clicked()
{
    QColor color = QColorDialog::getColor(Qt::white);
    updateColor(2, color);
}

void Settings::on_NavbarHover_clicked()
{
    QColor color = QColorDialog::getColor(Qt::white);
    updateColor(3, color);
}

void Settings::on_NavbarSelected_clicked()
{
    QColor color = QColorDialog::getColor(Qt::white);
    updateColor(4, color);
}

void Settings::on_TitleBarColor_clicked()
{
    QColor color = QColorDialog::getColor(Qt::white);
    updateColor(5, color);
}

void Settings::on_ResetColors_clicked()
{
    QSettings palette(QSettings::IniFormat, QSettings::UserScope, "Horizon Launcher", "palette");

    if (palette.isWritable())
    {
        palette.beginGroup("Primary");
        palette.setValue("ActiveElement", "#FFFFFF");
        palette.setValue("InactiveSelection", "#686868");
        palette.setValue("HoverSelection", "#ADADAD");
        palette.setValue("DarkElement", "#4D5250");
        palette.setValue("LightText", "#FFFFFF");
        palette.setValue("DarkText", "#242424");
        palette.setValue("SubText", "#B2B4B3");
        palette.setValue("PrimaryBase", "#282828");
        palette.setValue("SecondaryBase", "#1F1F1F");
        palette.setValue("TertiaryBase", "#131313");
        palette.setValue("DarkestBase", "#0F0F0F");
        palette.endGroup();

        palette.beginGroup("Body");
        palette.setValue("Background", "#212121");
        palette.endGroup();

        palette.beginGroup("Navbar");
        palette.setValue("Background", "#111111");
        palette.setValue("SelectedColor", "#9351E5");
        palette.setValue("HoverColor", "#ADADAD");
        palette.endGroup();

        palette.beginGroup("TitleBar");
        palette.setValue("Color", "#F5F6F7");
        palette.endGroup();
    }

    ui->BodyColor->setStyleSheet("background-color: " + palette.value("Body/Background").toString() + ";}");
    ui->NavbarBG->setStyleSheet("background-color: " + palette.value("Navbar/Background").toString() + ";}");
    ui->NavbarHover->setStyleSheet("background-color: " + palette.value("Navbar/HoverColor").toString() + ";}");
    ui->NavbarSelected->setStyleSheet("background-color: " + palette.value("Navbar/SelectedColor").toString() + ";}");
    ui->TitleBarColor->setStyleSheet("background-color: " + palette.value("TitleBar/Color").toString() + ";}");
}

void Settings::updateColor(int id, QColor color)
{
    if (!color.isValid())
    {
        return;
    }

    QSettings palette(QSettings::IniFormat, QSettings::UserScope, "Horizon Launcher", "palette");

    if (palette.isWritable())
    {
        if (id == 1) // Body/Background
        {
            palette.beginGroup("Body");
            palette.setValue("Background", color.name());
            palette.endGroup();

            ui->BodyColor->setStyleSheet("background-color: " + color.name() + ";}");
        }
        else if (id <= 4) // Navbar
        {
            palette.beginGroup("Navbar");
            if (id == 2) // Background
            {
                palette.setValue("Background", color.name());
                ui->NavbarBG->setStyleSheet("background-color: " + color.name() + ";}");
            }
            else if (id == 3) // Hover
            {
                palette.setValue("HoverColor", color.name());
                ui->NavbarHover->setStyleSheet("background-color: " + color.name() + ";}");
            }
            else if (id == 4) // Selected
            {
                palette.setValue("SelectedColor", color.name());
                ui->NavbarSelected->setStyleSheet("background-color: " + color.name() + ";}");
            }
            palette.endGroup();
        }
        else if (id == 5)
        {
            palette.beginGroup("TitleBar");
            palette.setValue("Color", color.name());
            palette.endGroup();
            ui->TitleBarColor->setStyleSheet("background-color: " + color.name() + ";}");
        }
    }
}

void Settings::on_ClearDatabaseButton_clicked()
{
    int ret = QMessageBox(QMessageBox::Question, "Deleting Database", "Proceeding will delete the database, the database will be non-recoverable. Proceed?", QMessageBox::Yes | QMessageBox::No).exec();
    switch (ret)
    {
        case QMessageBox::Yes:
            db.reset();
            break;
        case QMessageBox::No:
            break;
        default:
            break;
    }
}

Settings::~Settings()
{
    delete ui;
}
