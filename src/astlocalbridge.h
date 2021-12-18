#ifndef ASTLOCALBRIDGE_H
#define ASTLOCALBRIDGE_H

#include <QString>

struct AstLocalBridge
{
    QString chanOneUuid;
    QString chanTwoUuid;
    QString bridgeOneUuid;
    QString bridgeTwoUuid;
    QStringList bridgeOneChanUuids;
    QStringList bridgeTwoChanUuids;
};

#endif // ASTLOCALBRIDGE_H
