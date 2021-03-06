#ifndef _LIBRARY_H_
#define _LIBRARY_H_

class LibraryGridView;
class LibraryCarouselView;

#include "Database.h"
#include "GameLauncher.h"
#include "BuddyList.h"
#include "LibraryGridView.h"
#include "LibraryDetailView.h"
#include "LibraryCarouselView.h"

#include <QWidget>
#include <QProcess>
#include <QSettings>
#include <QDir>
#include <QGridLayout>
#include <QList>
#include <QStackedWidget>

/** Library class.
 * Class to handle the library section of the launcher
*/
class Library : public QWidget
{
    Q_OBJECT

public:
    Library(QSettings* p, QWidget* parent = 0);

    void launchGame(QString gameName);
    void changeLaunchOpts(QString gameName);

    static Database db;

public slots:
    void setGridView();
    void setDetailView();
    void setCarouselView();

private:
    GameLauncher gl;
    QGridLayout* mainLayout;
    LibraryGridView* gridView;
    LibraryDetailView* detailView;
    LibraryCarouselView* carouselView;
    QLineEdit* searchBox;
    QStackedWidget* viewStack;

    void init(QSettings* p);
};

#endif
