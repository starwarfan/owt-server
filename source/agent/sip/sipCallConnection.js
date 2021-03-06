// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

'use strict';
var SipGateway = require('../sipIn/build/Release/sipIn');
var frameAddon = require('../rtcFrame/build/Release/rtcFrame.node');
var MediaFrameMulticaster = require(
    '../mediaFrameMulticaster/build/Release/mediaFrameMulticaster');

var AudioFrameConstructor = frameAddon.AudioFrameConstructor;
var AudioFramePacketizer = frameAddon.AudioFramePacketizer;
var VideoFrameConstructor = frameAddon.VideoFrameConstructor;
var VideoFramePacketizer = frameAddon.VideoFramePacketizer;

var path = require('path');
var logger = require('../logger').logger;
var log = logger.getLogger('SipCallConnection');
exports.SipCallConnection = function (spec, onMediaUpdate) {
    var that = {},
        input = true,
        output = true,
        gateway = spec.gateway,
        peerURI = spec.peerURI,
        audio = spec.audio,
        video = spec.video,
        support_red = spec.red,
        support_ulpfec = spec.ulpfec,
        audioFrameConstructor,
        audioFramePacketizer,
        videoFrameConstructor,
        videoFramePacketizer,
        sip_callConnection;

    var dispatcher;

    sip_callConnection = new SipGateway.SipCallConnection(gateway, peerURI);
    if (audio) {
        // sip->mcu
        audioFrameConstructor = new AudioFrameConstructor();
        audioFrameConstructor.bindTransport(sip_callConnection);

        // mcu->sip
        audioFramePacketizer = new AudioFramePacketizer();
        audioFramePacketizer.bindTransport(sip_callConnection);
    }
    if (video) {
        videoFrameConstructor = new VideoFrameConstructor(function (mediaUpdate) {
            onMediaUpdate(peerURI, 'in', mediaUpdate);
        });
        videoFrameConstructor.bindTransport(sip_callConnection);

        const transportExt = -1;
        const mid = '';
        const midExt = -1;
        const selfRequestKeyFrame = true;
        videoFramePacketizer = new VideoFramePacketizer(
            support_red, support_ulpfec, transportExt, mid, midExt, selfRequestKeyFrame);
        videoFramePacketizer.bindTransport(sip_callConnection);
    }

    that.close = function () {
        log.debug('SipCallConnection close');
        if (audio && audioFramePacketizer) {
            audioFramePacketizer.unbindTransport();
            audioFramePacketizer.close();
            audioFramePacketizer = undefined;
        }

        if (video && videoFramePacketizer) {
            videoFramePacketizer.unbindTransport();
            videoFramePacketizer.close();
            videoFramePacketizer = undefined;
        }

        if (audio && audioFrameConstructor) {
            audioFrameConstructor.unbindTransport();
            audioFrameConstructor.close();
            audioFrameConstructor = undefined;
        }

        if (video && videoFrameConstructor) {
            videoFrameConstructor.unbindTransport();
            videoFrameConstructor.close();
            videoFrameConstructor = undefined;
        }

        if (dispatcher) {
            dispatcher.close();
        }

        sip_callConnection && sip_callConnection.close();
        sip_callConnection = undefined;
        log.debug('Completely close');
    };

    that.addDestination = function (track, dest) {
        if (audio && track === 'audio' && audioFrameConstructor) {
            audioFrameConstructor.addDestination(dest);
            return;
        } else if (video && track === 'video' && videoFrameConstructor) {
            videoFrameConstructor.addDestination(dest);
            return;
        }

        log.warn('Wrong track:'+track);
    };

    that.removeDestination = function (track, dest) {
        if (audio && track === 'audio' && audioFrameConstructor) {
            audioFrameConstructor.removeDestination(dest);
            return;
        } else if (video && track === 'video' && videoFrameConstructor) {
            videoFrameConstructor.removeDestination(dest);
            return;
        }

        log.warn('Wrong track:'+track);
    };

    that.receiver = function (track) {
        if (audio && track === 'audio') {
            return audioFramePacketizer;
        }

        if (video && track === 'video') {
            return videoFramePacketizer;
        }

        log.error('receiver error');
        return undefined;
    };

    that.requestKeyFrame = function() {
        if (video && videoFrameConstructor)
            videoFrameConstructor.requestKeyFrame();
    };

    that.source = function() {
        if (!dispatcher) {
            dispatcher = new MediaFrameMulticaster();
            if (video && videoFrameConstructor) {
                videoFrameConstructor.addDestination(dispatcher);
            }
            if (audio && audioFrameConstructor) {
                audioFrameConstructor.addDestination(dispatcher);
            }
        }
        return dispatcher.source();
    }

    return that;
};
