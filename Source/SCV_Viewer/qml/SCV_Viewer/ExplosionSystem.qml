import QtQuick 2.0
import QtQuick.Particles 2.0

Rectangle {
    width: 60
    height: 60
    color:  "black"

    ParticleSystem {
        anchors.fill : parent
        // renders a tiny image
        ImageParticle {
            source : "Particles/resources/particle.png"
        }
        // emit particle object with a size of 20 pixels
        Emitter {
            anchors.fill: parent
            size: 20
        }
    }
}
