/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtQuick.Particles 2.0

ParticleSystem {
    id: particlesSystem
    anchors.fill: parent

    property alias emitterX: emitter.x
    property alias emitterY: emitter.y
    property alias velocity: emitter.velocity

    property alias color: img.color
    property alias emitting: emitter.enabled
    ImageParticle {
        id: img
        source: "blur-circle.png"
        colorVariation: 0.1
        color: "black"
        alpha: 0.1
    }
    Emitter {
        id: emitter
        velocityFromMovement: 10
        emitRate: 80
        lifeSpan: 500
        velocity: PointDirection{ y: -90; yVariation: 20; }
        acceleration: PointDirection{ xVariation: 100; yVariation: 90; }
        size: 10
        sizeVariation: 12
        endSize: 15
        enabled: false
    }
    SequentialAnimation {
        id: animation
        PropertyAction { target: emitter; property: "enabled"; value: true }
        NumberAnimation { target: emitter; property: "lifeSpan"; easing.type: Easing.InQuad;
            from: 500; to: 0; duration: 10000 }
        PropertyAction { target: emitter; property: "enabled"; value: false }
    }
    states: [
        State {
            name: "On"
            PropertyChanges {
                target: particlesSystem
                emitting: true
                color: "#ff521d"
            }
        },
        State {
            name: "Off"
            PropertyChanges {
                target: particlesSystem
                emitting: true
                color: "#101010"
            }
            PropertyChanges {
                target: img
                alpha: 0.5
            }
        },
        State {
            name: "StandBy"
            PropertyChanges {
                target: particlesSystem
                emitting: true
                color: "#101010"
            }
        }
    ]
    transitions: [
        Transition {
            ColorAnimation { duration: 1500 }
            NumberAnimation { duration: 1500 }
        }
    ]
    onStateChanged: {
        animation.start()
    }
}
