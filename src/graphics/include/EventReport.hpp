/*
 * This file is part of Insight.
 * Copyright (C) 2018 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
 *
 * Insight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Insight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Insight.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EVENTREPORT_HPP
#define EVENTREPORT_HPP

/** Report obtained after evaluating if an event happened on some input devices. */
struct EventReport {
    /** Flag indicating if the event happened or not. */
    bool happened;
    /**
     * Intensity of the event.
     *
     * Analog devices (mouse) will provide something proportional to the input.
     * Discrete inputs (ex: keyboard button) will produce 0 or 1.
     */
    float intensity;
};

#endif /* EVENTREPORT_HPP */
