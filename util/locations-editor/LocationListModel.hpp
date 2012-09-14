/*
 * Stellarium Location List Editor
 * Copyright (C) 2012  Bogdan Marinov
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef LOCATIONLISTMODEL_HPP
#define LOCATIONLISTMODEL_HPP

#include <QAbstractTableModel>

#include <QFile>
#include <QList>
#include <QMap>
#include <QString>

#include "Location.hpp"

//! Custom Model class to store the locations list.
//! Location objects are stored in QList instead of a QMap as it is in
//! Stellarium, which may lead to some problems...
//! On the other hand, it allows editing duplicate entries.
//! @author Bogdan Marinov
class LocationListModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	explicit LocationListModel(QObject *parent = 0);
	~LocationListModel();
	
	static LocationListModel* load(QFile* file);
	bool save(QFile* file);
	bool saveBinary(QIODevice *file);
	
	bool isModified() const {return wasModified;}
	
	// Reimplemented:
	int columnCount(const QModelIndex& parent = QModelIndex() ) const;
	int rowCount(const QModelIndex& parent = QModelIndex() ) const;
	
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	
	//! Delete the location and re-trigger duplicate ID calculation.
	void removeLocation(int row);
	
	
	//! Messages logged during loading: duplicates, invalid lines, etc.
	QString loadingLog;
	
signals:
	void modified(bool modified);
	
public slots:
	void setModified(bool changed = true);
	
private:
	//! Flag set to "true" if the model has been modified.
	bool wasModified;
	//! The location list stored by the model.
	QList<Location*> locations;
	//! Similar to the one in StelLocationMgr, but a multi-map.
	QMap<QString,Location*> stelIds;
	
	//! Comment lines from the block at the beginning of the list file.
	QList<QString> leadingComments;
	//! Aggregated comment lines from the body of the list file.
	//! Written at the end of the file when saving.
	QList<QString> comments;
	
	//! Line number of the last detected duplicate original, used for logging.
	int lastDupLine;
	
	//! Check for unique ID, add the location to the map and mark duplicates.
	//! Tries to avoid duplicate IDs by expanding them.
	//! @param skipDuplicates if true, the argument is deleted if
	//! there's already a location with that ID.
	//! @returns 0 if the argument has been deleted as a duplicate.
	Location* addLocationId(Location* loc, bool skipDuplicates = false);
	
	//! Update duplicate status.
	bool updateDuplicates(Location* loc);
	
	//! Check if the index is within the model.
	bool isValidIndex(const QModelIndex& index) const;
};

#endif // LOCATIONLISTMODEL_HPP
