// Experimental HID mapping for the Native Instruments Traktor Kontrol X1. Do not use!
// Based on the Z1 HID mapping of Janek Fischer < https://github.com/xeruf/mixxx-controller-mappings >.

function KontrolX1Controller() {
    this.controller = new HIDController()

    this.controller.softTakeoverAll()

    this.mode = 'default'

    // region CONTROLS

    this.registerInputPackets = function() {
        var packet = new HIDPacket('control', 0x1)

        // Knobs. 2 bytes per knob.
        packet.addControl('hid', 'knob_00', 1, 'H') // Knob 0,0
        packet.addControl('hid', 'knob_01', 3, 'H') // Knob 0,1
        packet.addControl('hid', 'knob_10', 5, 'H') // Knob 1,0
        packet.addControl('hid', 'knob_11', 7, 'H') // Knob 1,1
        packet.addControl('hid', 'knob_20', 9, 'H') // Knob 2,0
        packet.addControl('hid', 'knob_21', 11, 'H') // Knob 2,1
        packet.addControl('hid', 'knob_30', 13, 'H') // Knob 3,0
        packet.addControl('hid', 'knob_31', 15, 'H') // Knob 3,1

        // Endless knobs. 2 bytes per knob.
        packet.addControl('hid', 'knob_40', 17, 'H') // Knob 4,0
        packet.addControl('hid', 'knob_41', 19, 'H') // Knob 4,1
        packet.addControl('hid', 'knob_50', 21, 'H') // Knob 5,0
        packet.addControl('hid', 'knob_51', 23, 'H') // Knob 5,1

        // Buttons. 8 buttons per byte from byte 25 to 29.
        packet.addControl('hid', 'button_00', 25, 'B', 0x1) // Button 0,0
        packet.addControl('hid', 'button_01', 25, 'B', 0x2) // Button 0,1
        packet.addControl('hid', 'button_10', 25, 'B', 0x4) // Button 1,0
        packet.addControl('hid', 'button_11', 25, 'B', 0x8) // Button 1,1
        packet.addControl('hid', 'button_20', 25, 'B', 0x10) // Button 2,0
        packet.addControl('hid', 'button_21', 25, 'B', 0x20) // Button 2,1
        packet.addControl('hid', 'button_30', 25, 'B', 0x40) // Button 3,0
        packet.addControl('hid', 'button_31', 25, 'B', 0x80) // Button 3,1

        packet.addControl('hid', 'button_40', 26, 'B', 0x1) // Button 4,0
        packet.addControl('hid', 'button_41', 26, 'B', 0x2) // Button 4,1
        packet.addControl('hid', 'button_42', 26, 'B', 0x4) // Button 4,2

        packet.addControl('hid', 'button_50', 26, 'B', 0x8) // Button 5,0
        packet.addControl('hid', 'button_51', 26, 'B', 0x10) // Button 5,1
        packet.addControl('hid', 'button_52', 26, 'B', 0x20) // Button 5,2
        packet.addControl('hid', 'button_53', 26, 'B', 0x40) // Button 5,3

        packet.addControl('hid', 'button_60', 26, 'B', 0x80) // Button 6,0
        packet.addControl('hid', 'button_61', 27, 'B', 0x1) // Button 6,1
        packet.addControl('hid', 'button_62', 27, 'B', 0x2) // Button 6,2

        packet.addControl('hid', 'button_70', 27, 'B', 0x4) // Button 7,0
        packet.addControl('hid', 'button_71', 27, 'B', 0x8) // Button 7,1
        packet.addControl('hid', 'button_72', 27, 'B', 0x10) // Button 7,2
        packet.addControl('hid', 'button_73', 27, 'B', 0x20) // Button 7,3

        packet.addControl('hid', 'button_80', 27, 'B', 0x40) // Button 8,0
        packet.addControl('hid', 'button_81', 27, 'B', 0x80) // Button 8,1
        packet.addControl('hid', 'button_82', 28, 'B', 0x1) // Button 8,2
        packet.addControl('hid', 'button_83', 28, 'B', 0x2) // Button 8,3

        packet.addControl('hid', 'button_90', 28, 'B', 0x4) // Button 9,0
        packet.addControl('hid', 'button_91', 28, 'B', 0x8) // Button 9,1
        packet.addControl('hid', 'button_92', 28, 'B', 0x10) // Button 9,2
        packet.addControl('hid', 'button_93', 28, 'B', 0x20) // Button 9,3

        packet.addControl('hid', 'button_100', 28, 'B', 0x40) // Button 10,0
        packet.addControl('hid', 'button_101', 28, 'B', 0x80) // Button 10,1
        packet.addControl('hid', 'button_102', 29, 'B', 0x1) // Button 10,2
        packet.addControl('hid', 'button_103', 29, 'B', 0x2) // Button 10,3

        this.controller.registerInputPacket(packet)
    }

    this.registerCallbacks = function() {
        HIDDebug('Registering HID callbacks')

        var controller = this.controller

        // Set link modifiers.
        controller.linkModifier('hid', 'button_41', 'shift')
        controller.linkModifier('hid', 'button_61', 'hotcue')

        // Hotcue 1.
        controller.setCallback('control', 'hid', 'button_00',
        function(button) {
                if (controller.modifiers.get('shift')) {
                    controller.toggle('[Channel1]', 'hotcue_1_clear')
                } else {
                    controller.toggle('[Channel1]', 'hotcue_1_activate')
                }
            }
        )

        // Hotcue 1.
        controller.setCallback('control', 'hid', 'button_01',
        function(button) {
                if (controller.modifiers.get('shift')) {
                    controller.toggle('[Channel2]', 'hotcue_1_clear')
                } else {
                    controller.toggle('[Channel2]', 'hotcue_1_activate')
                }
            }
        )

        // Hotcue 2.
        controller.setCallback('control', 'hid', 'button_10',
        function(button) {
                if (controller.modifiers.get('shift')) {
                    controller.toggle('[Channel1]', 'hotcue_2_clear')
                } else {
                    controller.toggle('[Channel1]', 'hotcue_2_activate')
                }
            }
        )

        // Hotcue 2.
        controller.setCallback('control', 'hid', 'button_11',
        function(button) {
                if (controller.modifiers.get('shift')) {
                    controller.toggle('[Channel2]', 'hotcue_2_clear')
                } else {
                    controller.toggle('[Channel2]', 'hotcue_2_activate')
                }
            }
        )

        // Hotcue 3.
        controller.setCallback('control', 'hid', 'button_20',
        function(button) {
                if (controller.modifiers.get('shift')) {
                    controller.toggle('[Channel1]', 'hotcue_3_clear')
                } else {
                    controller.toggle('[Channel1]', 'hotcue_3_activate')
                }
            }
        )

        // Hotcue 3.
        controller.setCallback('control', 'hid', 'button_21',
        function(button) {
                if (controller.modifiers.get('shift')) {
                    controller.toggle('[Channel2]', 'hotcue_3_clear')
                } else {
                    controller.toggle('[Channel2]', 'hotcue_3_activate')
                }
            }
        )

        // Hotcue 4.
        controller.setCallback('control', 'hid', 'button_30',
        function(button) {
                if (controller.modifiers.get('shift')) {
                    controller.toggle('[Channel1]', 'hotcue_4_clear')
                } else {
                    controller.toggle('[Channel1]', 'hotcue_4_activate')
                }
            }
        )

        // Hotcue 4.
        controller.setCallback('control', 'hid', 'button_31',
        function(button) {
                if (controller.modifiers.get('shift')) {
                    controller.toggle('[Channel2]', 'hotcue_4_clear')
                } else {
                    controller.toggle('[Channel2]', 'hotcue_4_activate')
                }
            }
        )

        // Load track.
        controller.setCallback('control', 'hid', 'button_40', function(button) {
                controller.toggle('[Channel1]', 'LoadSelectedTrack')
        })

        // Load track.
        controller.setCallback('control', 'hid', 'button_42', function(button) {
                controller.toggle('[Channel2]', 'LoadSelectedTrack')
        })

        // Actvate beatloop.
        controller.setCallback('control', 'hid', 'button_60', function(button) {
                controller.toggle('[Channel1]', 'beatloop_activate')
        })

        // Activate beatloop.
        controller.setCallback('control', 'hid', 'button_62', function(button) {
                controller.toggle('[Channel2]', 'beatloop_activate')
        })

        // Loop in.
        controller.setCallback('control', 'hid', 'button_70', function(button) {
                controller.toggle('[Channel1]', 'loop_in')
        })

        // Loop in.
        controller.setCallback('control', 'hid', 'button_72', function(button) {
                controller.toggle('[Channel2]', 'loop_in')
        })

        // Loop out.
        controller.setCallback('control', 'hid', 'button_71', function(button) {
                controller.toggle('[Channel1]', 'loop_out')
        })

        // Loop out.
        controller.setCallback('control', 'hid', 'button_73', function(button) {
                controller.toggle('[Channel2]', 'loop_out')
        })

        // Beatjump backward.
        controller.setCallback('control', 'hid', 'button_80', function(button) {
                controller.toggle('[Channel1]', 'beatjump_0.25_backward')
        })

        // Beatjump backward.
        controller.setCallback('control', 'hid', 'button_82', function(button) {
                controller.toggle('[Channel2]', 'beatjump_0.25_backward')
        })

        // Beatjump forward.
        controller.setCallback('control', 'hid', 'button_81', function(button) {
                controller.toggle('[Channel1]', 'beatjump_0.25_forward')
        })

        // Beatjump forward.
        controller.setCallback('control', 'hid', 'button_83', function(button) {
                controller.toggle('[Channel2]', 'beatjump_0.25_forward')
        })

        // Cue.
        controller.setCallback('control', 'hid', 'button_90',
                function(button) {
                        if (controller.modifiers.get('shift')) {
                            controller.toggle('[Channel1]', 'cue_clear')
                        } else {
                            controller.toggle('[Channel1]', 'cue_default')
                        }
                    }
        )

        // Cue.
        controller.setCallback('control', 'hid', 'button_92',
                function(button) {
                        if (controller.modifiers.get('shift')) {
                            controller.toggle('[Channel2]', 'cue_clear')
                        } else {
                            controller.toggle('[Channel2]', 'cue_default')
                        }
                    }
        )

        // Cup.
        controller.setCallback('control', 'hid', 'button_91', function(button) {
                controller.toggle('[Channel1]', 'cue_play')
        })

        // Cup.
        controller.setCallback('control', 'hid', 'button_93', function(button) {
                controller.toggle('[Channel2]', 'cue_play')
        })

        // Play.
        controller.setCallback('control', 'hid', 'button_100', function(button) {
            if (button.value === controller.buttonStates.pressed) {
                controller.toggle('[Channel1]', 'play')
            }
        })

        // Play.
        controller.setCallback('control', 'hid', 'button_102', function(button) {
            if (button.value === controller.buttonStates.pressed) {
                controller.toggle('[Channel2]', 'play')
            }
        })

        // Sync.
        controller.setCallback('control', 'hid', 'button_101', function(button) {
                controller.toggle('[Channel1]', 'beatsync')
        })

        // Sync.
        controller.setCallback('control', 'hid', 'button_103', function(button) {
                controller.toggle('[Channel2]', 'beatsync')
        })

        // Gain.
        this.linkKnob('default', 'knob_00', '[Channel1]', 'pregain', 'zeroOneScaling')
        controller.setCallback('control', 'hid', 'knob_00', this.knob)

        // Gain.
        this.linkKnob('default', 'knob_01', '[Channel2]', 'pregain', 'zeroOneScaling')
        controller.setCallback('control', 'hid', 'knob_01', this.knob)

        // High.
        this.linkKnob('default', 'knob_10', '[EqualizerRack1_[Channel1]_Effect1]', 'parameter3', 'zeroOneScaling')
        controller.setCallback('control', 'hid', 'knob_10', this.knob)

        // High.
        this.linkKnob('default', 'knob_11', '[EqualizerRack1_[Channel2]_Effect1]', 'parameter3', 'zeroOneScaling')
        controller.setCallback('control', 'hid', 'knob_11', this.knob)

        // Mid.
        this.linkKnob('default', 'knob_20', '[EqualizerRack1_[Channel1]_Effect1]', 'parameter2', 'zeroOneScaling')
        controller.setCallback('control', 'hid', 'knob_20', this.knob)

        // Mid.
        this.linkKnob('default', 'knob_21', '[EqualizerRack1_[Channel2]_Effect1]', 'parameter2', 'zeroOneScaling')
        controller.setCallback('control', 'hid', 'knob_21', this.knob)

        // Low.
        this.linkKnob('default', 'knob_30', '[EqualizerRack1_[Channel1]_Effect1]', 'parameter1', 'zeroOneScaling')
        controller.setCallback('control', 'hid', 'knob_30', this.knob)

        // Low.
        this.linkKnob('default', 'knob_31', '[EqualizerRack1_[Channel2]_Effect1]', 'parameter1', 'zeroOneScaling')
        controller.setCallback('control', 'hid', 'knob_31', this.knob)

        // Note: The encoders had to be adapted, as it is unclear if the implementation of the Z1 mapping allowed for a shift mapping.

        // Browse library.
        controller.setCallback('control', 'hid', 'knob_40',
        function(field) {
            if (controller.modifiers.get('shift')) {
                engine.setParameter('[Channel1]', 'beatjump', engine.getValue('[Channel1]', 'beatjump_size') *  KontrolX1.scalers['browseScaling_1'](field.value))
            } else {
                engine.setParameter('[Library]', 'MoveVertical', KontrolX1.scalers['browseScaling_0'](field.value))
            }
        })

        // Browse library.
        controller.setCallback('control', 'hid', 'knob_41',
        function(field) {
            if (controller.modifiers.get('shift')) {
                engine.setParameter('[Channel2]', 'beatjump', engine.getValue('[Channel2]', 'beatjump_size') *  KontrolX1.scalers['browseScaling_1'](field.value))
            } else {
                engine.setParameter('[Library]', 'MoveVertical', KontrolX1.scalers['browseScaling_1'](field.value))
            }
        })

        // Set beatloops.
        controller.setCallback('control', 'hid', 'knob_50',
        function(field) {
            if (controller.modifiers.get('shift')) {
                // engine.setParameter('[Channel1]', 'beatloop_size', KontrolX1.scalers['loopScaling'](field.value))
            } else {
                engine.setParameter('[Channel1]', 'beatloop_size', KontrolX1.scalers['loopScaling'](field.value))
            }
        })

        // Set beatloops.
        controller.setCallback('control', 'hid', 'knob_51',
        function(field) {
            if (controller.modifiers.get('shift')) {
                // engine.setParameter('[Channel2]', 'beatloop_size', KontrolX1.scalers['loopScaling'](field.value))
            } else {
                engine.setParameter('[Channel2]', 'beatloop_size', KontrolX1.scalers['loopScaling'](field.value))
            }
        })

    }

    // endregion

    // region LIGHTS

    this.registerOutputPackets = function() {
        var packet = new HIDPacket('lights', 0x0C)

        packet.addOutput('hid', 'led_00', 8, 'B') // Button 0,0
        packet.addOutput('hid', 'led_01', 4, 'B') // Button 0,1
        packet.addOutput('hid', 'led_10', 7, 'B') // Button 1,0
        packet.addOutput('hid', 'led_11', 3, 'B') // Button 1,1
        packet.addOutput('hid', 'led_20', 6, 'B') // Button 2,0
        packet.addOutput('hid', 'led_21', 2, 'B') // Button 2,1
        packet.addOutput('hid', 'led_30', 5, 'B') // Button 3,0
        packet.addOutput('hid', 'led_31', 1, 'B') // Button 3,1

        packet.addOutput('hid', 'led_41', 29, 'B') // Button 4,1

        packet.addOutput('hid', 'led_50', 25, 'B') // Button 5,0
        packet.addOutput('hid', 'led_51', 26, 'B') // Button 5,1
        packet.addOutput('hid', 'led_52', 27, 'B') // Button 5,2
        packet.addOutput('hid', 'led_53', 28, 'B') // Button 5,3

        packet.addOutput('hid', 'led_61', 31, 'B') // Button 6,1

        packet.addOutput('hid', 'led_70', 18, 'B') // Button 7,0
        packet.addOutput('hid', 'led_71', 17, 'B') // Button 7,1
        packet.addOutput('hid', 'led_72', 16, 'B') // Button 7,2
        packet.addOutput('hid', 'led_73', 15, 'B') // Button 7,3

        packet.addOutput('hid', 'led_80', 20, 'B') // Button 8,1
        packet.addOutput('hid', 'led_81', 19, 'B') // Button 8,0
        packet.addOutput('hid', 'led_82', 14, 'B') // Button 8,2
        packet.addOutput('hid', 'led_83', 13, 'B') // Button 8,3

        packet.addOutput('hid', 'led_90', 22, 'B') // Button 9,0
        packet.addOutput('hid', 'led_91', 21, 'B') // Button 9,1
        packet.addOutput('hid', 'led_92', 12, 'B') // Button 9,2
        packet.addOutput('hid', 'led_93', 11, 'B') // Button 9,3
        //
        packet.addOutput('hid', 'led_100', 24, 'B') // Button 10,0
        packet.addOutput('hid', 'led_101', 23, 'B') // Button 10,1
        packet.addOutput('hid', 'led_102', 10, 'B') // Button 10,2
        packet.addOutput('hid', 'led_103', 9, 'B') // Button 10,3

        this.controller.registerOutputPacket(packet)
    }

    // endregion

}

var KontrolX1 = new KontrolX1Controller()

KontrolX1.init = function(id) {
    KontrolX1.id = id

    // Save values for encoders to calculate deltas.
    KontrolX1.prev_browse_0 = 0
    KontrolX1.prev_browse_1 = 0

    KontrolX1.registerInputPackets()
    KontrolX1.registerOutputPackets()
    KontrolX1.registerCallbacks()

    // Hotcue 1.
    KontrolX1.controller.connectLight('[Channel1]', 'hotcue_1_enabled', function(value, packet, group, name) {
        packet.getField('hid', 'led_00').value = value * 0x7F
    })

    // Hotcue 1.
    KontrolX1.controller.connectLight('[Channel2]', 'hotcue_1_enabled', function(value, packet, group, name) {
        packet.getField('hid', 'led_01').value = value * 0x7F
    })

    // Hotcue 2.
    KontrolX1.controller.connectLight('[Channel1]', 'hotcue_2_enabled', function(value, packet, group, name) {
        packet.getField('hid', 'led_10').value = value * 0x7F
    })

    // Hotcue 2.
    KontrolX1.controller.connectLight('[Channel2]', 'hotcue_2_enabled', function(value, packet, group, name) {
        packet.getField('hid', 'led_11').value = value * 0x7F
    })

    // Hotcue 3.
    KontrolX1.controller.connectLight('[Channel1]', 'hotcue_3_enabled', function(value, packet, group, name) {
        packet.getField('hid', 'led_20').value = value * 0x7F
    })

    // Hotcue 3.
    KontrolX1.controller.connectLight('[Channel2]', 'hotcue_3_enabled', function(value, packet, group, name) {
        packet.getField('hid', 'led_21').value = value * 0x7F
    })

    // Hotcue 4.
    KontrolX1.controller.connectLight('[Channel1]', 'hotcue_4_enabled', function(value, packet, group, name) {
        packet.getField('hid', 'led_30').value = value * 0x7F
    })

    // Hotcue 4.
    KontrolX1.controller.connectLight('[Channel2]', 'hotcue_4_enabled', function(value, packet, group, name) {
        packet.getField('hid', 'led_31').value = value * 0x7F
    })

    // Loop in.
    KontrolX1.controller.connectLight('[Channel1]', 'loop_in', function(value, packet, group, name) {
        packet.getField('hid', 'led_70').value = value * 0x7F
    })

    // Loop in.
    KontrolX1.controller.connectLight('[Channel2]', 'loop_in', function(value, packet, group, name) {
        packet.getField('hid', 'led_72').value = value * 0x7F
    })

    // Loop out.
    KontrolX1.controller.connectLight('[Channel1]', 'loop_out', function(value, packet, group, name) {
        packet.getField('hid', 'led_71').value = value * 0x7F
    })

    // Loop out.
    KontrolX1.controller.connectLight('[Channel2]', 'loop_out', function(value, packet, group, name) {
        packet.getField('hid', 'led_73').value = value * 0x7F
    })

    // Beatjump backward.
    KontrolX1.controller.connectLight('[Channel1]', 'beatjump_0.25_backward', function(value, packet, group, name) {
        packet.getField('hid', 'led_80').value = value * 0x7F
    })

    // Beatjump backward.
    KontrolX1.controller.connectLight('[Channel2]', 'beatjump_0.25_backward', function(value, packet, group, name) {
        packet.getField('hid', 'led_82').value = value * 0x7F
    })

    // Beatjump forward.
    KontrolX1.controller.connectLight('[Channel1]', 'beatjump_0.25_forward', function(value, packet, group, name) {
        packet.getField('hid', 'led_81').value = value * 0x7F
    })

    // Beatjump forward.
    KontrolX1.controller.connectLight('[Channel2]', 'beatjump_0.25_forward', function(value, packet, group, name) {
        packet.getField('hid', 'led_83').value = value * 0x7F
    })

    // Cue.
    KontrolX1.controller.connectLight('[Channel1]', 'cue_default', function(value, packet, group, name) {
        packet.getField('hid', 'led_90').value = value * 0x7F
    })

    // Cue.
    KontrolX1.controller.connectLight('[Channel2]', 'cue_default', function(value, packet, group, name) {
        packet.getField('hid', 'led_92').value = value * 0x7F
    })

    // Cup.
    KontrolX1.controller.connectLight('[Channel1]', 'cue_play', function(value, packet, group, name) {
        packet.getField('hid', 'led_91').value = value * 0x7F
    })

    // Cup.
    KontrolX1.controller.connectLight('[Channel2]', 'cue_play', function(value, packet, group, name) {
        packet.getField('hid', 'led_93').value = value * 0x7F
    })

    // Play.
    KontrolX1.controller.connectLight('[Channel1]', 'play', function(value, packet, group, name) {
        packet.getField('hid', 'led_100').value = value * 0x7F
    })

    // Play.
    KontrolX1.controller.connectLight('[Channel2]', 'play', function(value, packet, group, name) {
        packet.getField('hid', 'led_102').value = value * 0x7F
    })

    // Sync.
    KontrolX1.controller.connectLight('[Channel1]', 'beatsync', function(value, packet, group, name) {
        packet.getField('hid', 'led_101').value = value * 0x7F
    })

    // Sync.
    KontrolX1.controller.connectLight('[Channel2]', 'beatsync', function(value, packet, group, name) {
        packet.getField('hid', 'led_103').value = value * 0x7F
    })

    // Set scalers for encoders.
    this.scalers = {};
    this.scalers['noScaling'] = function(value) {
        return value;
    }
    this.scalers['zeroOneScaling'] = function(value) {
        return script.absoluteLin(value, 0, 1, 0, 4095);
    }
    this.scalers['browseScaling_0'] = function(value) {

        // Calculate delta and save new value.
        var delta = value - KontrolX1.prev_browse_0
        KontrolX1.prev_browse_0 = value

        // Treat jumps from 0 to 15 and 15 to 0 separately,
        if (delta == -15) {
            delta = 1
        }
        else if (delta == 15) {
            delta = -1
        }
        else if (delta > 0) {
            delta = 1
        }
        else if (delta < 0) {
            delta = -1
        }
        return delta;
    }
    this.scalers['browseScaling_1'] = function(value) {

        // Calculate delta and save new value.
        var delta = value - KontrolX1.prev_browse_1
        KontrolX1.prev_browse_1 = value

        // Treat jumps from 0 to 15 and 15 to 0 separately,
        if (delta == -15) {
            delta = 1
        }
        else if (delta == 15) {
            delta = -1
        }
        else if (delta > 0) {
            delta = 1
        }
        else if (delta < 0) {
            delta = -1
        }
        return delta;
    }
    this.scalers['loopScaling'] = function(value) {

        return Math.min(Math.pow(2, value), 128);
    }

    print('NI Traktor Kontrol X1 ' + KontrolX1.id + ' initialized!')
}


// region knobs

KontrolX1.knobs = {}
KontrolX1.linkKnob = function(mode, knob, group, name, scaler) {
    if (!(mode in KontrolX1.knobs))
        KontrolX1.knobs[mode] = {}
    KontrolX1.knobs[mode][knob] = {
        'mode': mode,
        'knob': knob,
        'group': group,
        'name': name,
        'scaler': scaler,
    }
}

KontrolX1.control = function(control, field) {
    if (control.callback !== undefined) {
        control.callback(control, field)
        return
    }
    var scaler = KontrolX1.scalers[control.scaler];
    engine.setParameter(control.group, control.name, scaler(field.value))
}

KontrolX1.knob = function(field) {
    var mode = KontrolX1.knobs[KontrolX1.mode]
    if (mode === undefined) {
        HIDDebug('Knob group not mapped in mode ' + KontrolX1.mode)
        return
    }
    var knob = mode[field.name]
    if (knob === undefined) {
        HIDDebug('Fader ' + field.name + ' not mapped in ' + KontrolX1.mode)
        return
    }
    return KontrolX1.control(knob, field)
}

// endregion

KontrolX1.shutdown = function() {
    KontrolX1.controller.getLightsPacket().clearControls()
    print('NI Traktor Kontrol X1 ' + KontrolX1.id + ' shut down!')
}

KontrolX1.incomingData = function(data, length) {
    KontrolX1.controller.parsePacket(data, length)
}
