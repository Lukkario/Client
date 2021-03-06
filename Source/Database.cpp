#include "Database.h"
#include "Defines.h"

#include <QDebug>
#include <QSqlError>

/** Database constructor
 * Constructs the local database.
 * Currently no interface to handle remote databases, just creates one in the
 * current working directory.
 */
Database::Database(QObject* parent)
{
    this->path = QDir(CONFIG_FOLDER).filePath("horizon.db");
}

Database::Database(QString path, QObject* parent)
{
    this->path = path;
}

/** Initialize the actual database, if it hasn't been done already.
 * \return Success/failure of the operation.
*/
bool Database::init()
{
    db = QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    db.setHostName("localhost");
    db.setDatabaseName(this->path);

    bool status = db.open();
    qDebug() << db.lastError();
    if (!status)
    {
        qDebug("Couldn't connect to the database!");
        return false;
    }

    QSqlQuery createQuery(db);

    bool rtn = createQuery.exec("CREATE TABLE IF NOT EXISTS games(ID INTEGER PRIMARY KEY ASC, GAMENAME TEXT NOT NULL, GAMEDIRECTORY TEXT NOT NULL, GAMEEXECUTABLE TEXT NOT NULL, ARGUMENTS TEXT NOT NULL, DRM INT DEFAULT 0);");

    if (rtn)
    {
        emit dbChanged();
    }

    return rtn;
}

/** Remove every table in the database.
 * \return Success/failure of the operation.
*/
bool Database::reset()
{
    QSqlQuery query(db);
    bool rtn = query.exec("DROP TABLE IF EXISTS games") && query.exec("CREATE TABLE IF NOT EXISTS games(ID INTEGER PRIMARY KEY ASC, GAMENAME TEXT NOT NULL, GAMEDIRECTORY TEXT NOT NULL, GAMEEXECUTABLE TEXT NOT NULL, ARGUMENTS TEXT NOT NULL, DRM INT DEFAULT 0);");

    if (rtn)
    {
        emit dbChanged();
    }

    return rtn;
}

/** Add a game to the database and repopulate the games list.
 * \param gameName The name of the game.
 * \param gameDirectory Working directory of the game.
 * \param executablePath The location of the executable on the filesystem.
 * \param arguments List of arguments to launch with.
 * \param drm The DRM the game came from, where 0 = None, 1 = Steam, 2 = Origin, 3 = uPlay
 * \return Success/failure of the operation.
*/
bool Database::addGame(QString gameName, QString gameDirectory, QString executablePath, QString arguments, int drm)
{
    QSqlQuery query(db);
    query.prepare("INSERT OR IGNORE INTO GAMES(GAMENAME, GAMEDIRECTORY, GAMEEXECUTABLE, ARGUMENTS, DRM) VALUES (:gameName, :gameDirectory, :executablePath, :arguments, :drm);");
    query.bindValue(":gameName", gameName);
    query.bindValue(":gameDirectory", gameDirectory);
    query.bindValue(":executablePath", executablePath);
    query.bindValue(":arguments", arguments);
    query.bindValue(":drm", drm);
    bool rtn = query.exec();

    if (rtn)
    {
        emit dbChanged();
    }
}

/** Add games to the database and repopulate the games list.
 * \param games GameList of games to add.
 * \return Success/failure of the operation.
*/
void Database::addGames(GameList games)
{
    for (auto& game : games)
    {
        addGame(game.gameName, game.gameDirectory, game.executablePath, game.arguments, game.drm);
    }

    emit dbChanged();
}

/** Remove a game from the database by their ID.
 * \param id ID of the game to remove.
 * \return Success/failure of the operation.
*/
bool Database::removeGameById(unsigned int id)
{
    if (std::get<0>(isExistant(id)))
    {
        QSqlQuery query(db);
        query.prepare("DELETE FROM games WHERE ID = :id;");
        query.bindValue(":id", id);
        bool rtn = query.exec();

        if (rtn)
        {
            emit dbChanged();
        }

        return rtn;
    }
    else
    {
        return false;
    }
}

/** Remove a game from the database by their name.
 * \param name Name of the game to remove.
*/
bool Database::removeGameByName(QString name)
{
    if (std::get<0>(isExistant(name)))
    {
        QSqlQuery query(db);
        query.prepare("DELETE FROM GAMES WHERE GAMENAME = :name;");
        query.bindValue(":name", name);
        bool rtn = query.exec();

        if (rtn)
        {
            emit dbChanged();
        }

        return rtn;
    }
    else
    {
        return false;
    }
}

/** Wrapper to access the Game object from the ID.
 * \param id ID to find.
 * \return A Game object, empty upon failure.
*/
Game Database::getGameById(unsigned int id)
{
    return std::get<1>(isExistant(id));
}

/** Wrapper to access the Game object from the name.
 * \param id ID to find.
 * \return A Game object, empty upon failure.
*/
Game Database::getGameByName(QString name)
{
    return std::get<1>(isExistant(name));
}

/** Perform a query to find a specific game in the database by their ID. Unsafe at the
 * moment.
 * \param id ID of the game to find.
 * \return A Game object upon success, 0 upon failure.
*/
std::pair<bool, Game> Database::isExistant(unsigned int id)
{
    QSqlQuery query(db);
    query.prepare("SELECT ID, GAMENAME, GAMEDIRECTORY, GAMEEXECUTABLE, ARGUMENTS, DRM FROM GAMES WHERE ID = :id;");
    query.bindValue(":id", id);
    query.exec();

    if (query.next())
    {
        QString name = query.value(1).toString();
        QString path = query.value(2).toString();
        QString exe = query.value(3).toString();
        QString args = query.value(4).toString();
        int drm = query.value(5).toInt();

        return std::make_pair(true, Game {id, name, path, exe, args, drm});
    }
    else
    {
        return std::make_pair(false, Game{});
    }
}

/** Perform a query to find a specific game by their name (soon to be
 * deprecated).
 *
 * \param name Name of the game to find.
 * \return A Game object upon success, 0 upon failure.
*/
std::pair<bool, Game> Database::isExistant(QString name)
{
    QSqlQuery query(db);
    query.prepare("SELECT ID, GAMEDIRECTORY, GAMEEXECUTABLE, ARGUMENTS, DRM FROM GAMES WHERE GAMENAME = :name;");
    query.bindValue(":name", name);
    query.exec();
    if (query.next())
    {
        unsigned int id = query.value(0).toUInt();
        QString path = query.value(1).toString();
        QString exe = query.value(2).toString();
        QString args = query.value(3).toString();
        int drm = query.value(4).toInt();

        return std::make_pair(true, Game {id, name, path, exe, args, drm});
    }
    else
    {
        return std::make_pair(false, Game{});
    }
}

/** Perform a query to find every game in the database.
 * \return A QList of Game objects containing everything in the database.
*/
QList<Game> Database::getGames()
{
    QList<Game> games;
    QSqlQuery query;
    query.exec("SELECT ID, GAMENAME, GAMEDIRECTORY, GAMEEXECUTABLE, ARGUMENTS, DRM FROM GAMES;");
    while (query.next())
    {
        unsigned int id = query.value(0).toUInt();
        QString name = query.value(1).toString();
        QString path = query.value(2).toString();
        QString exe = query.value(3).toString();
        QString args = query.value(4).toString();
        int drm = query.value(5).toInt();

        games.append({id, name, path, exe, args, drm});
    }
    return games;
}

/** Queries the database to find the number of games.
 * \return Total number of games stored so far.
*/
unsigned int Database::getGameCount() const
{
    QSqlQuery query(db);
    query.exec("SELECT count() FROM GAMES;");
    if (!query.next())
    {
        return 0;
    }

    return query.value(0).toUInt();
}

/** Sets the launch options of a game by ID.
 * \param id The ID of the game
 * \param launchOpts The new launch options
 * \return Success (true)/Failure (false) of the operation.
*/
bool Database::setLaunchOptionsById(unsigned int id, QString launchOpts)
{
    if (std::get<0>(isExistant(id)))
    {
        QSqlQuery query(db);
        query.prepare("UPDATE games SET `ARGUMENTS` = :newOpts WHERE ID = :id;");
        query.bindValue(":newOpts", launchOpts);
        query.bindValue(":id", id);
        bool rtn = query.exec();

        if (rtn)
        {
            emit dbChanged();
        }

        return rtn;
    }
    else
    {
        return false;
    }
}

/** Sets the launch options of a game by name.
 * \param name The name of the game (as stored in the database)
 * \param launchOpts The new launch options
 * \return Success (true)/Failure (false) of the operation.
*/
bool Database::setLaunchOptionsByName(QString name, QString launchOpts)
{
    if (std::get<0>(isExistant(name)))
    {
        QSqlQuery query(db);
        query.prepare("UPDATE games SET `ARGUMENTS` = :newOpts WHERE GAMENAME = :name;");
        query.bindValue(":newOpts", launchOpts);
        query.bindValue(":name", name);
        bool rtn = query.exec();

        if (rtn)
        {
            emit dbChanged();
        }

        return rtn;
    }
    else
    {
        return false;
    }
}
