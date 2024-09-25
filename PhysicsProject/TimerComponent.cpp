
#pragma once

#include "Component.h"
#include "Box2DWorld.h"
#include <SFML/System/Vector2.hpp>
#include "box2d/box2d.h"

TimerComponent::TimerComponent(float duration)
    : m_duration(duration), m_remainingTime(duration), m_isRunning(false) {}

void TimerComponent::update(float deltaTime) {
    if (m_isRunning && m_remainingTime > 0) {
        m_remainingTime -= deltaTime;
        if (m_remainingTime < 0) {
            m_remainingTime = 0;
        }
    }
}

void TimerComponent::start() {
    m_isRunning = true;
    m_remainingTime = m_duration;
}

void TimerComponent::pause() {
    m_isRunning = false;
}

void TimerComponent::resume() {
    m_isRunning = true;
}

void TimerComponent::reset() {
    m_remainingTime = m_duration;
    m_isRunning = false;
}

bool TimerComponent::isFinished() const {
    return m_remainingTime <= 0;
}

bool TimerComponent::isRunning() const {
    return m_isRunning;
}

float TimerComponent::getRemainingTime() const {
    return m_remainingTime;
}

float TimerComponent::getElapsedTime() const {
    return m_duration - m_remainingTime;
}

float TimerComponent::getDuration() const {
    return m_duration;
}
