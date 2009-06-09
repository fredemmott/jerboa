/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
 *   Copyright 2009 Frederick Emmott <mail@fredemmott.co.uk>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/
 
#ifndef TAG_CLOUD_MODEL_H
#define TAG_CLOUD_MODEL_H

#include <QAbstractItemModel>
#include <QMap>

class WeightedTag;

class TagCloudModel: public QAbstractItemModel
{
	public:
		enum CustomRoles
		{
			WeightRole       = Qt::UserRole,
			LinearWeightRole
		};
	
		TagCloudModel(QObject* parent = 0);
		~TagCloudModel();

		void setTags(const QList<WeightedTag>& tags);
	
		virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
		
		virtual QModelIndex parent(const QModelIndex&) const;
		virtual int rowCount(const QModelIndex& = QModelIndex()) const;
		virtual int columnCount(const QModelIndex& = QModelIndex()) const;
		virtual QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const;
		Qt::ItemFlags flags(const QModelIndex& index) const;
	protected:
		QMultiMap< qreal, QString> m_tagHash;
		QMultiMap< qreal, QString> m_logTagHash;
		qreal m_maxWeight;
		qreal m_minLogWeight;
};

#endif //TAG_CLOUD_MODEL_H

