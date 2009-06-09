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
 
#include "TagCloudModel.h"
#include "WeightedTag.h"

#include <QDebug>

#include <cmath>
#include <limits>

TagCloudModel::TagCloudModel(QObject* parent)
: QAbstractItemModel(parent)
{
}

void TagCloudModel::setTags(const QList<WeightedTag>& tags)
{
	m_tagHash.clear();
	m_logTagHash.clear();

	m_maxWeight = 0;


	m_minLogWeight = std::numeric_limits<qreal>::max();
	Q_FOREACH(const WeightedTag& tag, tags)
	{
		m_maxWeight = qMax(tag.weight(), m_maxWeight);
		qreal logWeight = std::log(tag.weight());
		m_minLogWeight = logWeight < m_minLogWeight ? logWeight : m_minLogWeight;
		m_logTagHash.insert(logWeight, tag.name());
		m_tagHash.insert(tag.weight(), tag.name());
	}
	reset();
}

TagCloudModel::~TagCloudModel()
{
}

int TagCloudModel::rowCount( const QModelIndex& parent ) const
{
	if(parent.isValid())
	{
		return 0;
	}
	
	return m_tagHash.count();
}

QVariant TagCloudModel::data( const QModelIndex& index, int role ) const
{
	switch(role)
	{
		case Qt::DisplayRole:
		{
			QMultiMap< qreal, QString >::const_iterator i = m_tagHash.constEnd();
			i -= index.row() + 1;
			return i.value();
		}
				
		case TagCloudModel::WeightRole:
		{
			QMultiMap< qreal, QString >::const_iterator i = m_tagHash.constEnd();
			i -= index.row() + 1;
			return QVariant::fromValue<qreal>((i.key() / m_maxWeight));
		}

		case TagCloudModel::LinearWeightRole:
		{
			QMultiMap< qreal, QString >::const_iterator i = m_logTagHash.constEnd();
			i -= index.row() + 1;
			return QVariant::fromValue<qreal>( ( i.key() - m_minLogWeight ) / ((m_logTagHash.constEnd() -1 ).key() - m_minLogWeight));
		}

		default:
			return QVariant();
	}
}

QModelIndex TagCloudModel::index( int row, int column, const QModelIndex& parent) const
{
	if(parent.isValid())
	{
		return QModelIndex();
	}

	return createIndex(row, column);
}

QModelIndex TagCloudModel::parent(const QModelIndex&) const
{
	return QModelIndex();
}

int TagCloudModel::columnCount(const QModelIndex&) const
{
	return 1;
}

Qt::ItemFlags TagCloudModel::flags(const QModelIndex & index) const
{
	Q_UNUSED( index );
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
