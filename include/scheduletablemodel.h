#ifndef SCHEDULETABLEMODEL_H
#define SCHEDULETABLEMODEL_H

#include <QAbstractTableModel>
#include "TimetableGenerator.h"

class ScheduleTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ScheduleTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setSchedule(const TimetableGenerator &generator);
    void clear();

private:
    struct ExamSlot {
        QString courseId;
        QString courseName;
        QString room;
        int students;
    };

    QVector<QVector<ExamSlot>> schedule; // [day][timeslot]
    int maxDays;
    int maxRooms;
};

#endif // SCHEDULETABLEMODEL_H