#ifndef SAM_MEASURESCOUNT_H
#define SAM_MEASURESCOUNT_H

#include <QVariantMap>

namespace SAM
{
    /**
     * @brief Estructura auxiliar para almacenar el número de elementos que hay en una
     *  batería. También sirve para ir contabilizando el avance de las mediciones.
     */
    struct MeasuresCount
    {
        unsigned int nPositions;
        unsigned int nGateways;
        unsigned int nLinks;
        unsigned int nLoops;
        unsigned int nChannels;
        unsigned int nMeasures;

        MeasuresCount() :
            nPositions(0),
            nGateways(0),
            nLinks(0),
            nLoops(0),
            nChannels(0),
            nMeasures(0)
        {}

        void clear()
        {
            nPositions = 0;
            nGateways = 0;
            nLinks = 0;
            nLoops = 0;
            nChannels = 0;
            nMeasures = 0;
        }

        unsigned int sum() const
        {
            return nPositions + nGateways + nLinks + nLoops + nChannels + nMeasures;
        }

        QVariantMap toVariant() const
        {
            QVariantMap ret;

            ret["nPositions"] = nPositions;
            ret["nGateways"] = nGateways;
            ret["nLinks"] = nLinks;
            ret["nLoops"] = nLoops;
            ret["nChannels"] = nChannels;
            ret["nMeasures"] = nMeasures;

            return ret;
        }
    };
}

#endif // SAM_MEASURESCOUNT_H
