// -*- coding: utf-8 -*-
// Copyright (C) 2006-2011 Rosen Diankov (rosen.diankov@gmail.com)
//
// This file is part of OpenRAVE.
// OpenRAVE is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
/** \file trajectory.h
    \brief  Definition of \ref OpenRAVE::TrajectoryBase

    Automatically included with \ref openrave.h
 */
#ifndef OPENRAVE_TRAJECTORY_H
#define OPENRAVE_TRAJECTORY_H

namespace OpenRAVE {

template <typename T> class RangeGenerator;

/// \brief bitmask options for Init API.
enum TrajectoryInitializationOptions
{
    TIO_ReserveTimeBasedVectors = 0x00000001, ///< If this is specified, reserve the time-based memories. If not specified, only reserve way-point-related memories. This is effective only when positive nWayPointsToReserve is given to Init argument.
};

enum TrajectorySerializeOptions
{
    TSO_SerializeAsXML = 0x8000, ///< On GenericTrajectory::serialize, if this is specified, the trajectory will serialized as XML, otherwise binary ortraj.
};

/** \brief <b>[interface]</b> Encapsulate a time-parameterized trajectories of robot configurations. <b>If not specified, method is not multi-thread safe.</b> \arch_trajectory
    \ingroup interfaces
 */
class OPENRAVE_API TrajectoryBase : public InterfaceBase
{
public:
    TrajectoryBase(EnvironmentBasePtr penv);
    virtual ~TrajectoryBase() {
    }

    /// \brief return the static interface type this class points to (used for safe casting)
    static inline InterfaceType GetInterfaceTypeStatic() {
        return PT_Trajectory;
    }

    /// \brief Initialize openrave trajectory. Many of APIs assume that this API is called beforehand.
    /// \param[in] nWayPointsToReserve : number of way points to reserve. If nWayPointsToReserve is positive value, internal memories are reserved. Otherwise, not reserved. Note that the waypoint-related memories which might grow in runtime APIs are reserved by nWayPointsToReserve.
    /// \param[in] options : options for Init. Please use TrajectoryInitializationOptions to specify the options.
    virtual void Init(const ConfigurationSpecification& spec, const int nWayPointsToReserve=0, const int options=0) = 0;

    /// \brief clears the waypoint data from the trajectory
    virtual void ClearWaypoints() = 0;

    /** \brief Sets/inserts new waypoints in the same configuration specification as the trajectory.

        \param index The index where to start modifying the trajectory.
        \param data The data to insert, can represent multiple consecutive waypoints. data.size()/GetConfigurationSpecification().GetDOF() waypoints are added.
        \param bOverwrite If true, will overwrite the waypoints starting at index, and will insert new waypoints only if end of trajectory is reached. If false, will insert the points before index: a 0 index inserts the new data in the beginning, a GetNumWaypoints() index inserts the new data at the end.
     */
    virtual void Insert(size_t index, const std::vector<dReal>& data, bool bOverwrite=false) = 0;

    /** \brief Sets/inserts new waypoints in a \b user-given configuration specification.

        \param index The index where to start modifying the trajectory.
        \param data The data to insert, can represent multiple consecutive waypoints. data.size()/GetConfigurationSpecification().GetDOF() waypoints are added.
        \param spec the specification in which the input data come in. Depending on what data is offered, some values of this trajectory's specification might not be initialized.
        \param bOverwrite If true, will overwrite the waypoints starting at index, and will insert new waypoints only if end of trajectory is reached; if the input spec does not overwrite all the data of the trjectory spec, then the original trajectory data will not be overwritten. If false, will insert the points before index: a 0 index inserts the new data in the beginning, a GetNumWaypoints() index inserts the new data at the end.
     */
    virtual void Insert(size_t index, const std::vector<dReal>& data, const ConfigurationSpecification& spec, bool bOverwrite=false) = 0;

    /** \brief Sets/inserts new waypoints in a \b user-given configuration specification.
        \param index The index where to start modifying the trajectory.
        \param pdata The data to insert, can represent multiple consecutive waypoints.
        \param nDataElements The number of data to insert, can represent multiple consecutive waypoints. nDataElements/GetConfigurationSpecification().GetDOF() waypoints are added.
        \param spec the specification in which the input data come in. Depending on what data is offered, some values of this trajectory's specification might not be initialized.
        \param bOverwrite If true, will overwrite the waypoints starting at index, and will insert new waypoints only if end of trajectory is reached; if the input spec does not overwrite all the data of the trjectory spec, then the original trajectory data will not be overwritten. If false, will insert the points before index: a 0 index inserts the new data in the beginning, a GetNumWaypoints() index inserts the new data at the end.
     */
    virtual void Insert(size_t index, const dReal* pdata, size_t nDataElements, const ConfigurationSpecification& spec, bool bOverwrite=false) = 0;

    /** \brief Sets/inserts new waypoints in the same configuration specification as the trajectory.

        \param index The index where to start modifying the trajectory.
        \param data The data to insert.
        \param nDataElements The number of data to insert, can represent multiple consecutive waypoints. nDataElements/GetConfigurationSpecification().GetDOF() waypoints are added.
        \param bOverwrite If true, will overwrite the waypoints starting at index, and will insert new waypoints only if end of trajectory is reached. If false, will insert the points before index: a 0 index inserts the new data in the beginning, a GetNumWaypoints() index inserts the new data at the end.
     */
    virtual void Insert(size_t index, const dReal* pdata, size_t nDataElements, bool bOverwrite=false) = 0;

    /// \brief removes a range of waypoints [startindex, endindex) removing starting at startindex and ending at the element before endindex.
    virtual void Remove(size_t startindex, size_t endindex) = 0;

    /** \brief samples a data point on the trajectory at a particular time

        \param data[out] the sampled point
        \param time[in] the time to sample
     */
    virtual void Sample(std::vector<dReal>& data, dReal time) const = 0;

    /** \brief samples a data point on the trajectory at a particular time and returns data for the group specified.

        The default implementation is slow, so interface developers should override it.
        \param data[out] the sampled point
        \param time[in] the time to sample
        \param spec[in] the specification format to return the data in
        \param reintializeData[in] if true, then data will be reset with 0s before sampling the trajectory. Otherwise, the data will be used as is
     */
    virtual void Sample(std::vector<dReal>& data, dReal time, const ConfigurationSpecification& spec, bool reintializeData=true) const;

    /** \brief bulk samples the trajectory given a vector of times using the trajectory's specification.

        \param data[out] the sampled points depending on the times
        \param times[in] the times to sample
     */
    virtual void SamplePoints(std::vector<dReal>& data, const std::vector<dReal>& times) const;

    /** \brief bulk samples the trajectory given a vector of times and a specific configuration specification.

        The default implementation is slow, so interface developers should override it.
        \param data[out] the sampled points for every time entry.
        \param times[in] the times to sample
        \param spec[in] the specification format to return the data in
     */
    virtual void SamplePoints(std::vector<dReal>& data, const std::vector<dReal>& times, const ConfigurationSpecification& spec) const;

    /** \brief bulk samples the trajectory evenly given a delta time using the trajectory's specification.

        \param data[out] the sampled points depending on the times
        \param deltatime[in] the delta time to sample
        \param ensureLastPoint[in] if true, data at duration of trajectory is sampled
     */
    virtual void SamplePointsSameDeltaTime(std::vector<dReal>& data, dReal deltatime, bool ensureLastPoint) const;

    /** \brief bulk samples the trajectory evenly given a delta time and a specific configuration specification.

        The default implementation is slow, so interface developers should override it.
        \param data[out] the sampled points for every time entry.
        \param deltatime[in] the delta time to sample
        \param ensureLastPoint[in] if true, data at duration of trajectory is sampled
        \param spec[in] the specification format to return the data in
     */
    virtual void SamplePointsSameDeltaTime(std::vector<dReal>& data, dReal deltatime, bool ensureLastPoint, const ConfigurationSpecification& spec) const;

    /** \brief bulk samples the trajectory evenly given a delta time and a specific configuration specification.

        The default implementation is slow, so interface developers should override it.
        \param data[out] the sampled points for every time entry.
        \param deltatime[in] the delta time to sample
        \param startTime[in] start time to sample from
        \param stopTime[in] stop time to sample to
        \param ensureLastPoint[in] if true, data at duration of trajectory is sampled
     */
    virtual void SampleRangeSameDeltaTime(std::vector<dReal>& data, dReal deltatime, dReal startTime, dReal stopTime, bool ensureLastPoint) const;

    /** \brief bulk samples the trajectory evenly given a delta time and a specific configuration specification.

        The default implementation is slow, so interface developers should override it.
        \param data[out] the sampled points for every time entry.
        \param deltatime[in] the delta time to sample
        \param startTime[in] start time to sample from
        \param stopTime[in] stop time to sample to
        \param ensureLastPoint[in] if true, data at duration of trajectory is sampled
        \param spec[in] the specification format to return the data in
     */
    virtual void SampleRangeSameDeltaTime(std::vector<dReal>& data, dReal deltatime, dReal startTime, dReal stopTime, bool ensureLastPoint, const ConfigurationSpecification& spec) const;
    virtual const ConfigurationSpecification& GetConfigurationSpecification() const = 0;

    /// \brief return the number of waypoints
    virtual size_t GetNumWaypoints() const = 0;

    /** \brief return a set of waypoints in the range [startindex,endindex)

        \param startindex[in] the start index of the waypoint (included)
        \param endindex[in] the end index of the waypoint (not included)
        \param data[out] the data of the waypoint
     */
    virtual void GetWaypoints(size_t startindex, size_t endindex, std::vector<dReal>& data) const = 0;

    /** \brief return a set of waypoints in the range [startindex,endindex) in a different configuration specification.

        The default implementation is very slow, so trajectory developers should really override it.
        \param startindex[in] the start index of the waypoint (included)
        \param endindex[in] the end index of the waypoint (not included)
        \param spec[in] the specification to return the data in
        \param data[out] the data of the waypoint
     */
    virtual void GetWaypoints(size_t startindex, size_t endindex, std::vector<dReal>& data, const ConfigurationSpecification& spec) const;

    /** \brief returns one waypoint

        \param index[in] index of the waypoint. If < 0, then counting starts from the last waypoint. For example GetWaypoints(-1,data) returns the last waypoint.
        \param data[out] the data of the waypoint
     */
    inline void GetWaypoint(int index, std::vector<dReal>& data) const
    {
        int numpoints = GetNumWaypoints();
        BOOST_ASSERT(index >= -numpoints && index < numpoints);
        if( index < 0 ) {
            index += numpoints;
        }
        GetWaypoints(index,index+1,data);
    }

    /** \brief returns one waypoint

        \param index[in] index of the waypoint. If < 0, then counting starts from the last waypoint. For example GetWaypoints(-1,data) returns the last waypoint.
        \param data[out] the data of the waypoint
     */
    inline void GetWaypoint(int index, std::vector<dReal>& data, const ConfigurationSpecification& spec) const
    {
        int numpoints = GetNumWaypoints();
        BOOST_ASSERT(index >= -numpoints && index < numpoints);
        if( index < 0 ) {
            index += numpoints;
        }
        GetWaypoints(index,index+1,data,spec);
    }

    /// \brief returns the nearest waypoint index that is before the designated time.
    ///
    /// If time is before the first waypoint's time, then will return 0. If time >= GetDuration(), will return GetNumWaypoints()
    /// If trajectory doesn't have time or is not initialized, will throw an exception
    virtual size_t GetFirstWaypointIndexAfterTime(dReal time) const = 0;

    /// \brief return the duration of the trajectory in seconds
    virtual dReal GetDuration() const = 0;

    /// \brief output the trajectory in XML format
    virtual void serialize(std::ostream& O, int options=0) const;

    /// \brief initialize the trajectory
    virtual void deserialize(std::istream& I);

    /// \brief initialize the trajectory via a raw pointer to memory
    virtual void DeserializeFromRawData(const uint8_t* pdata, size_t nDataSize);

    /// \brief Clone the contents of the given trajectory to the current trajectory.
    /// \param preference the interface whose information to clone
    /// \param cloningoptions mask of CloningOptions
    virtual void Clone(InterfaceBaseConstPtr preference, int cloningoptions) override;

    /// \brief swap the contents of the data between the two trajectories.
    ///
    /// \param traj the trajrectory to swap data with. this->GetXMLId() == traj->GetXMLId() has to be met.
    /// This function is meant to be extremely fast with as few memory copies as possible.
    virtual void Swap(TrajectoryBasePtr traj) OPENRAVE_DUMMY_IMPLEMENTATION;

protected:
    inline TrajectoryBasePtr shared_trajectory() {
        return boost::static_pointer_cast<TrajectoryBase>(shared_from_this());
    }
    inline TrajectoryBaseConstPtr shared_trajectory_const() const {
        return boost::static_pointer_cast<TrajectoryBase const>(shared_from_this());
    }

    /// \brief sample points in time range
    ///
    /// \param data[out] the sampled points for every time entry.
    /// \param timeRange[in] time range to sample points at
    template <typename T>
    void _SamplePointsInRange(std::vector<dReal>& data, RangeGenerator<T>& timeRange) const;

    /// \brief sample points in time range
    ///
    /// \param data[out] the sampled points for every time entry.
    /// \param timeRange[in] time range to sample points at
    /// \param spec[in] the specification to return the data in
    template <typename T>
    void _SamplePointsInRange(std::vector<dReal>& data, RangeGenerator<T>& timeRange, const ConfigurationSpecification& spec) const;

private:
    virtual const char* GetHash() const override {
        return OPENRAVE_TRAJECTORY_HASH;
    }
};

} // end namespace OpenRAVE

#endif // TRAJECTORY_H
