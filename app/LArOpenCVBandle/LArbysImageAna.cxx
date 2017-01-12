#ifndef __LARBYSIMAGEANA_CXX__
#define __LARBYSIMAGEANA_CXX__
#include "LArbysImageAna.h"

#include "AlgoData/DefectClusterData.h"
#include "AlgoData/Refine2DVertexData.h"
#include "AlgoData/VertexClusterData.h"
#include "AlgoData/LinearVtxFilterData.h"
#include "AlgoData/dQdXProfilerData.h"
#include "AlgoData/DefectClusterData.h"
#include "AlgoData/LinearTrackClusterData.h"
#include "AlgoData/SingleShowerData.h"
#include "LArUtil/GeometryHelper.h"
#include "LArUtil/LArProperties.h"

namespace larcv {

  static LArbysImageAnaProcessFactory __global_LArbysImageAnaProcessFactory__;

  LArbysImageAna::LArbysImageAna(const std::string name)
    : ProcessBase(name),
      _event_tree(nullptr),
      _hip_event_tree(nullptr),
      _hip_plane_tree(nullptr),
      _defect_event_tree(nullptr),
      _defect_plane_tree(nullptr),
      _defect_defect_tree(nullptr),
      _vtx3d_event_tree(nullptr),
      _vtx3d_tree(nullptr),      
      _particle_tree(nullptr),
      _track_tree(nullptr),
      _shower_tree(nullptr)
  {}
    
  void LArbysImageAna::configure(const PSet& cfg)
  {
    _hipcluster_name         = cfg.get<std::string>("HIPClusterAlgoName");
    _defectcluster_name      = cfg.get<std::string>("DefectClusterAlgoName");
    _pcacandidates_name      = cfg.get<std::string>("PCACandidatesAlgoName");
    _refine2dvertex_name     = cfg.get<std::string>("Refine2DVertexAlgoName");
    _vertexcluster_name      = cfg.get<std::string>("VertexTrackClusterAlgoName");
    _linearvtxfilter_name    = cfg.get<std::string>("LinearVtxFilterAlgoName");
    _dqdxprofiler_name       = cfg.get<std::string>("dQdXProfilerAlgoName");
    _lineartrackcluster_name = cfg.get<std::string>("LinearTrackClusterAlgoName");
    _vertexsingleshower_name = cfg.get<std::string>("VertexSingleShowerAlgoName");
  }

  void LArbysImageAna::ClearHIPCluster() {
    _npx_v.clear();
    _q_sum_v.clear();
    _q_avg_v.clear();
    _is_hip_v.clear(); 
    _pixel_v.clear();
    _mip_pixel_v.clear();
    _hip_pixel_v.clear();
  }

  
  void LArbysImageAna::ClearDefect() {
    _defect_atomic_len_v.clear(); 
    _defect_atomic_qsum_v.clear(); 
    _defect_atomic_npts_v.clear();    
    _defect_atomic_qavg_v.clear();
  }
  
  void LArbysImageAna::ClearParticle() {
    _qsum_v.clear();
    _npix_v.clear();
    _num_atoms_v.clear();
    _start_x_v.clear();
    _start_y_v.clear();
    _end_x_v.clear();
    _end_y_v.clear();
    _start_end_length_v.clear();
    _atom_sum_length_v.clear();
    _first_atom_cos_v.clear();
    _dqdx_vv.clear();
    _dqdx_start_idx_vv.clear();
  }
  
  void LArbysImageAna::ClearEvent()
  {}
  
  void LArbysImageAna::ClearVertex() {
    _vtx2d_x_v.clear();
    _vtx2d_y_v.clear();
    _circle_x_v.clear();
    _circle_y_v.clear();
    _num_clusters_v.clear();
    _num_pixels_v.clear();
    _num_pixel_frac_v.clear();
    
    _circle_vtx_r_v.clear();
    _circle_vtx_angle_v.clear();

    _vtx2d_x_v.resize(3);
    _vtx2d_y_v.resize(3);
    _circle_x_v.resize(3);
    _circle_y_v.resize(3);
    _circle_xs_v.resize(3);
    _num_clusters_v.resize(3);
    _num_pixels_v.resize(3);
    _num_pixel_frac_v.resize(3);
    _circle_vtx_r_v.resize(3);
    _circle_vtx_angle_v.resize(3);
  }

  void LArbysImageAna::ClearTracks() {

    _edge2D_1_x_v.clear();
    _edge2D_1_y_v.clear();
    _edge2D_2_x_v.clear();
    _edge2D_2_y_v.clear();

    _edge2D_1_x_v.resize(3);
    _edge2D_1_y_v.resize(3);
    _edge2D_2_x_v.resize(3);
    _edge2D_2_y_v.resize(3);
    
  }

  void LArbysImageAna::ClearShowers() {

    _start2D_x_v.clear();
    _start2D_y_v.clear();
    _dir2D_x_v.clear();
    _dir2D_y_v.clear();

    _start2D_x_v.resize(3);
    _start2D_y_v.resize(3);
    _dir2D_x_v.resize(3);
    _dir2D_y_v.resize(3);
    
  }
  
  void LArbysImageAna::initialize()
  {
    
    _event_tree = new TTree("EventTree","");

    _event_tree->Branch("run"    ,&_run    , "run/i");
    _event_tree->Branch("subrun" ,&_subrun , "subrun/i");
    _event_tree->Branch("event"  ,&_event  , "event/i");

    /// HIP per event cluster data
    _hip_event_tree = new TTree("ClusterEventTree","");
    _hip_event_tree->Branch("run"    ,&_run    , "run/i");
    _hip_event_tree->Branch("subrun" ,&_subrun , "subrun/i");
    _hip_event_tree->Branch("event"  ,&_event  , "event/i");
    _hip_event_tree->Branch("hip_per_plane",&_hip_per_plane,"hip_per_plane/i");
    _hip_event_tree->Branch("num_hip",&_num_hip_plane,"num_hip/i");
    _hip_event_tree->Branch("num_mip",&_num_mip_plane,"num_mip/i");
    _hip_event_tree->Branch("avg_long_hip_length",&_avg_long_hip_length,"avg_long_hip_length/F");
    _hip_event_tree->Branch("avg_long_mip_length",&_avg_long_mip_length,"avg_long_mip_length/F");
    
    _hip_event_tree->Branch("avg_long_mip_width",&_avg_long_mip_width,"avg_long_mip_width/F");
    _hip_event_tree->Branch("avg_long_hip_width",&_avg_long_hip_width,"avg_long_hip_width/F");
    _hip_event_tree->Branch("avg_long_mip_qsum",&_avg_long_mip_qsum ,"avg_long_mip_qsum/F");
    _hip_event_tree->Branch("avg_long_hip_qsum",&_avg_long_hip_qsum ,"avg_long_hip_qsum/F");
    _hip_event_tree->Branch("avg_long_mip_qavg",&_avg_long_mip_qavg ,"avg_long_mip_qavg/F");
    _hip_event_tree->Branch("avg_long_hip_qavg",&_avg_long_hip_qavg ,"avg_long_hip_qavg/F");
    _hip_event_tree->Branch("avg_long_mip_npx",&_avg_long_mip_npx  ,"avg_long_mip_npx/F");
    _hip_event_tree->Branch("avg_long_hip_npx",&_avg_long_hip_npx  ,"avg_long_hip_npx/F");

    /// HIP per plane cluster data
    _hip_plane_tree = new TTree("ClusterPlaneTree","");
    _hip_plane_tree->Branch("run"    ,&_run    , "run/i");
    _hip_plane_tree->Branch("subrun" ,&_subrun , "subrun/i");
    _hip_plane_tree->Branch("event"  ,&_event  , "event/i");
    _hip_plane_tree->Branch("plane", &_hip_cluster_plane, "plane/i");
    _hip_plane_tree->Branch("num_mips", &_num_mips, "num_mips/i");
    _hip_plane_tree->Branch("num_hips", &_num_hips, "num_hips/i");
    _hip_plane_tree->Branch("npx_v"  , &_npx_v);
    _hip_plane_tree->Branch("q_sum_v", &_q_sum_v);
    _hip_plane_tree->Branch("q_avg_v", &_q_avg_v);
    _hip_plane_tree->Branch("is_hip_v", &_is_hip_v);
    _hip_plane_tree->Branch("long_hip_length",&_long_hip_length,"long_hip_length/F");
    _hip_plane_tree->Branch("long_mip_length",&_long_mip_length,"long_mip_length/F");
    _hip_plane_tree->Branch("pixel_v",&_pixel_v);
    _hip_plane_tree->Branch("mip_pixel_v",&_mip_pixel_v);
    _hip_plane_tree->Branch("hip_pixel_v",&_hip_pixel_v);
    
    _hip_plane_tree->Branch("long_mip_width",&_long_mip_width,"long_mip_width/F");
    _hip_plane_tree->Branch("long_hip_width",&_long_hip_width,"long_hip_width/F");
    _hip_plane_tree->Branch("long_mip_qsum",&_long_mip_qsum,"long_mip_qsum/F");
    _hip_plane_tree->Branch("long_hip_qsum",&_long_hip_qsum,"long_hip_qsum/F");
    _hip_plane_tree->Branch("long_mip_qavg",&_long_mip_qavg,"long_mip_qavg/F");
    _hip_plane_tree->Branch("long_hip_qavg",&_long_hip_qavg,"long_hip_qavg/F");
    _hip_plane_tree->Branch("long_mip_npx",&_long_mip_npx,"long_mip_npx/F");
    _hip_plane_tree->Branch("long_hip_npx",&_long_hip_npx,"long_hip_npx/F");
    _hip_plane_tree->Branch("long_hip_angle",&_long_hip_angle,"long_hip_angle/F");
    _hip_plane_tree->Branch("long_mip_angle",&_long_mip_angle,"long_mip_angle/F");
    
    /// Defect Event Tree
    _defect_event_tree = new TTree("DefectEventTree", "");
    
    _defect_event_tree->Branch("run"     ,&_run    , "run/i");
    _defect_event_tree->Branch("subrun"  ,&_subrun , "subrun/i");
    _defect_event_tree->Branch("event"   ,&_event  , "event/i");
    _defect_event_tree->Branch("ndefects",&_defect_n_defects,"ndefects/i");

    /// Defect Plane Tree
    _defect_plane_tree = new TTree("DefectPlaneTree", "");
    _defect_plane_tree->Branch("run"    ,&_run    , "run/i");
    _defect_plane_tree->Branch("subrun" ,&_subrun , "subrun/i");
    _defect_plane_tree->Branch("event"  ,&_event  , "event/i");
    _defect_plane_tree->Branch("defect_plane_id",  &_defect_plane_id,  "defect_plane_id/i");
    _defect_plane_tree->Branch("defect_has_match",  &_defect_has_match,  "defect_has_match/i");       
    _defect_plane_tree->Branch("defect_matched_x", &_defect_matched_x, "defect_matched_x/F");
    _defect_plane_tree->Branch("defect_matched_y", &_defect_matched_y, "defect_matched_y/F");
    _defect_plane_tree->Branch("defect_matched_d", &_defect_matched_d, "defect_matched_d/F");
    _defect_plane_tree->Branch("defect_matched_id",&_defect_matched_id,"defect_matched_id/i");
    
    /// Defect->Atomic Info
    
    _defect_defect_tree = new TTree("DefectDefectTree", "");
    
    _defect_defect_tree->Branch("run"    ,&_run    , "run/i");
    _defect_defect_tree->Branch("subrun" ,&_subrun , "subrun/i");
    _defect_defect_tree->Branch("event"  ,&_event  , "event/i");
    _defect_defect_tree->Branch("defect_plane_id"       ,&_defect_plane_id, "defect_plane_id/i");   
    _defect_defect_tree->Branch("defect_id"             ,&_defect_id, "defect_id/i");
    _defect_defect_tree->Branch("defect_dist_start_end" ,&_defect_dist_start_end, "defect_dist_start_end/D");
    _defect_defect_tree->Branch("defect_dist"           ,&_defect_dist, "defect_dist/D");
    _defect_defect_tree->Branch("defect_n_atomics"      ,&_defect_n_atomics, "defect_n_atomics/i");
    _defect_defect_tree->Branch("defect_atomic_len_v"   ,&_defect_atomic_len_v);
    _defect_defect_tree->Branch("defect_atomic_qsum_v"  ,&_defect_atomic_qsum_v);
    _defect_defect_tree->Branch("defect_atomic_ntps_v"  ,&_defect_atomic_npts_v);
    _defect_defect_tree->Branch("defect_atomic_qavg_v"  ,&_defect_atomic_qavg_v);
    
    /// VertexTrackCluster
    _vtx3d_event_tree = new TTree("Vtx3DEventTree","");
    _vtx3d_event_tree->Branch("run"    ,&_run    , "run/i");
    _vtx3d_event_tree->Branch("subrun" ,&_subrun , "subrun/i");
    _vtx3d_event_tree->Branch("event"  ,&_event  , "event/i");
    _vtx3d_event_tree->Branch("n_vtx3d", &_n_vtx3d, "n_vtx3d/i");
    
    _vtx3d_tree = new TTree("Vtx3DTree","");

    /// Unique event keys
    _vtx3d_tree->Branch("run"    ,&_run    , "run/i");
    _vtx3d_tree->Branch("subrun" ,&_subrun , "subrun/i");
    _vtx3d_tree->Branch("event"  ,&_event  , "event/i");

    _vtx3d_tree->Branch("vtx3d_id", &_vtx3d_id, "vtx3d_id/i");
    _vtx3d_tree->Branch("vtx3d_type", &_vtx3d_type, "vtx3d_type/i");
    
    _vtx3d_tree->Branch("vtx3d_x", &_vtx3d_x, "vtx3d_x/D"  );
    _vtx3d_tree->Branch("vtx3d_y", &_vtx3d_y, "vtx3d_y/D"  );
    _vtx3d_tree->Branch("vtx3d_z", &_vtx3d_z, "vtx3d_z/D"  );

    _vtx3d_tree->Branch("vtx2d_x_v", &_vtx2d_x_v );
    _vtx3d_tree->Branch("vtx2d_y_v", &_vtx2d_y_v );

    _vtx3d_tree->Branch("circle_vtx_x_v",&_circle_x_v);
    _vtx3d_tree->Branch("circle_vtx_y_v",&_circle_y_v);
    _vtx3d_tree->Branch("circle_vtx_xs_v",&_circle_xs_v);

    _vtx3d_tree->Branch("num_planes"       , &_num_planes, "num_planes/i");
    
    _vtx3d_tree->Branch("num_clusters_v"   , &_num_clusters_v);
    _vtx3d_tree->Branch("num_pixels_v"     , &_num_pixels_v);
    _vtx3d_tree->Branch("num_pixel_frac_v" , &_num_pixel_frac_v);
    _vtx3d_tree->Branch("sum_pixel_frac"   ,&_sum_pixel_frac ,"sum_pixel_frac/d");
    _vtx3d_tree->Branch("prod_pixel_frac"   ,&_prod_pixel_frac ,"prod_pixel_frac/d");
    
    //LinearVtxFilter
    _vtx3d_tree->Branch("circle_vtx_r_v",&_circle_vtx_r_v);
    _vtx3d_tree->Branch("circle_vtx_angle_v",&_circle_vtx_angle_v);

    //Particle Tree
    _particle_tree = new TTree("ParticleTree","");

    _particle_tree->Branch("run"    ,&_run    , "run/i");
    _particle_tree->Branch("subrun" ,&_subrun , "subrun/i");
    _particle_tree->Branch("event"  ,&_event  , "event/i");

    _particle_tree->Branch("vtx3d_id", &_vtx3d_id, "vtx3d_id/i");
    _particle_tree->Branch("plane_id", &_plane_id, "plane_id/i");
    _particle_tree->Branch("n_pars",&_n_pars,"n_pars/i");

    _particle_tree->Branch("qsum_v",&_qsum_v);
    _particle_tree->Branch("npix_v",&_npix_v);

    _particle_tree->Branch("num_atoms_v",&_num_atoms_v);
    _particle_tree->Branch("start_x_v",&_start_x_v);
    _particle_tree->Branch("start_y_v",&_start_y_v);
    _particle_tree->Branch("end_x_v",&_end_x_v);
    _particle_tree->Branch("end_y_v",&_end_y_v);
    _particle_tree->Branch("start_end_length_v",&_start_end_length_v);
    _particle_tree->Branch("atom_sum_length_v",&_atom_sum_length_v);
    _particle_tree->Branch("first_atom_cos_v",&_first_atom_cos_v);
    _particle_tree->Branch("dqdx_vv",&_dqdx_vv);
    _particle_tree->Branch("dqdx_start_idx_vv",&_dqdx_start_idx_vv);

    /// LinearTrackCluster info
    _track_tree = new TTree("TrackTree","");
    
    _event_tree->Branch("n_trackclusters",&_n_trackclusters,"_n_trackclusters/i");

    _track_tree->Branch("run"    ,&_run    , "run/i");
    _track_tree->Branch("subrun" ,&_subrun , "subrun/i");
    _track_tree->Branch("event"  ,&_event  , "event/i");

    _track_tree->Branch("edge2D_1_x_v", &_edge2D_1_x_v);
    _track_tree->Branch("edge2D_1_y_v", &_edge2D_1_y_v);
    _track_tree->Branch("edge2D_2_x_v", &_edge2D_2_x_v);
    _track_tree->Branch("edge2D_2_y_v", &_edge2D_2_y_v);
    
    /// VertexSingleShower info
    _shower_tree = new TTree("ShowerTree","");
    
    _event_tree->Branch("n_showerclusters",&_n_showerclusters,"_n_showerclusters/i");

    _shower_tree->Branch("run"    ,&_run    , "run/i");
    _shower_tree->Branch("subrun" ,&_subrun , "subrun/i");
    _shower_tree->Branch("event"  ,&_event  , "event/i");

    _shower_tree->Branch("id", &_shower_id, "_shower_id/i");
    _shower_tree->Branch("ass_id", &_shower_ass_id, "_ass_id/i");
    _shower_tree->Branch("ass_type", &_shower_ass_type, "_ass_type/i");

    _shower_tree->Branch("vtx3D_x",&_shower_vtx3D_x,"_shower_vtx3D_x/d");
    _shower_tree->Branch("vtx3D_y",&_shower_vtx3D_y,"_shower_vtx3D_y/d");
    _shower_tree->Branch("vtx3D_z",&_shower_vtx3D_z,"_shower_vtx3D_z/d");
    
    _shower_tree->Branch("start2D_x_v", &_start2D_x_v);
    _shower_tree->Branch("start2D_y_v", &_start2D_y_v);
    
    _shower_tree->Branch("dir2D_x_v", &_dir2D_x_v);
    _shower_tree->Branch("dir2D_y_v", &_dir2D_y_v);

  }


  float LArbysImageAna::dist_defect_hip_mip(const larocv::data::Cluster* c1,
					    const larocv::data::Cluster* c2,
					    const geo2d::Vector<float> dpt) {

    if (!c1) return kINVALID_FLOAT;
    if (!c2) return kINVALID_FLOAT;
    
    const auto& c1_pts = c1->px();
    const auto& c2_pts = c2->px();

    double dist;
    
    //calculate closest distance to c1
    float max_dist1 = kINVALID_FLOAT;
    dist=0;
    // std::cout << "Checking MIP" << std::endl;
    for(const auto & pt : c1_pts) {
      dist = geo2d::dist(pt,dpt);
      // std::cout << "{ " << pt << " -- " << dpt << "} ==> "<< dist << std::endl;
      if (dist < max_dist1) max_dist1 = dist;
    }
    
    // std::cout << "Got min distance: " <<max_dist1 << std::endl;

    // std::cout << "Checking HIP" << std::endl;
    //calculate closest distance to c2
    float max_dist2 = kINVALID_FLOAT;
    dist=0;
    for(const auto & pt : c1_pts) {
      dist = geo2d::dist(pt,dpt);
      if (dist < max_dist2) max_dist2 = dist;
      //std::cout << "{ " << pt << " -- " << dpt << "} ==> "<< dist << std::endl;
    }
    //std::cout << "Got min distance: " <<max_dist2 << std::endl;
    //std::cout << "Returning " << (max_dist1 + max_dist2) / 2.0 << std::endl;
    return (max_dist1 + max_dist2) / 2.0;
    
  }
  
  bool LArbysImageAna::process(IOManager& mgr)
  {
    ClearEvent();
    
    LARCV_DEBUG() << "process" << std::endl;

    /// Unique event keys
    const auto& event_id = mgr.event_id();
    _run    = (uint) event_id.run();
    _subrun = (uint) event_id.subrun();
    _event  = (uint) event_id.event();

    const auto& dm  = _mgr_ptr->DataManager();    

    std::vector<larocv::data::Cluster*> longest_mip_cluster_ptr_v(3,nullptr);
    std::vector<larocv::data::Cluster*> longest_hip_cluster_ptr_v(3,nullptr);

    /// HIP cluster data
    auto hipcluster_id = dm.ID(_hipcluster_name);
    if (hipcluster_id != larocv::kINVALID_ALGO_ID ) {
      auto hipctor_data = (larocv::data::HIPClusterData*)dm.Data( dm.ID(_hipcluster_name) );

      _hip_per_plane=0;
      _num_hip_plane=0;
      _num_mip_plane=0;

      _avg_long_hip_length=0;
      _avg_long_mip_length=0;
      
      float was_hip=0;
      float was_mip=0;

      _avg_long_hip_width = 0;
      _avg_long_mip_width = 0;
      _avg_long_hip_qsum = 0;
      _avg_long_mip_qsum = 0;
      _avg_long_hip_qavg = 0;
      _avg_long_mip_qavg = 0;
      _avg_long_hip_npx = 0;
      _avg_long_mip_npx = 0;
      _avg_long_hip_length = 0;
      _avg_long_mip_length = 0;
      
      for(uint plane_id=0;plane_id<3;++plane_id) {
	auto& hipctor_plane_data = hipctor_data->_plane_data_v[plane_id];

	_long_hip_width = kINVALID_FLOAT;
	_long_mip_width = kINVALID_FLOAT;
	_long_hip_qsum = kINVALID_FLOAT;
	_long_mip_qsum = kINVALID_FLOAT;
	_long_hip_qavg = kINVALID_FLOAT;
	_long_mip_qavg = kINVALID_FLOAT;
	_long_hip_npx = kINVALID_FLOAT;
	_long_mip_npx = kINVALID_FLOAT;
	_long_hip_length = kINVALID_FLOAT;
	_long_mip_length = kINVALID_FLOAT;
	_long_hip_angle = kINVALID_FLOAT;
	_long_mip_angle = kINVALID_FLOAT;
	
	uint num_hips=0;
	uint num_mips=0;
	
	_hip_cluster_plane = plane_id;

	_long_hip_length=0;
	_long_mip_length=0;
	
	larocv::data::Cluster* long_mip_cluster = nullptr;
	larocv::data::Cluster* long_hip_cluster = nullptr;
	float mip_dist = 0;
	float hip_dist = 0;
	
	for(auto& cluster : hipctor_plane_data.get_clusters()) {
	  if (cluster.iship()) {
	    if (cluster.length() > hip_dist) {
	      hip_dist = cluster.length();
	      long_hip_cluster = & cluster;
	    }
	    num_hips++;
	  }
	  if (cluster.ismip()) {
	    if (cluster.length() > mip_dist) {
	      mip_dist = cluster.length();
	      long_mip_cluster = & cluster;
	    }
	    num_mips++;
	  }
	  
	  _npx_v.push_back(cluster.npx());
	  _q_sum_v.push_back(cluster.qsum());
	  _q_avg_v.push_back(cluster.qavg());
	  _is_hip_v.push_back((uint)cluster.iship());
	  
	  _pixel_v.push_back((std::vector<float>) cluster.pixel_v());
	  _mip_pixel_v.push_back( (std::vector<float>) cluster.mip_pixel_v() );
	  _hip_pixel_v.push_back( (std::vector<float>) cluster.hip_pixel_v() );
	}

	_long_hip_length = hip_dist;
	_long_mip_length = mip_dist;
	
	_avg_long_hip_length += hip_dist;
	_avg_long_mip_length += mip_dist;
	
	_num_mips = num_mips;
	_num_hips = num_hips;

	_num_hip_plane+=num_hips;
	_num_mip_plane+=num_mips;

	if (long_mip_cluster) {
	  _long_mip_width = long_mip_cluster->width();
	  _avg_long_mip_width += _long_mip_width;
	  _long_mip_qsum = long_mip_cluster->qsum();
	  _avg_long_mip_qsum += _long_mip_qsum;
	  _long_mip_qavg = long_mip_cluster->qavg();
	  _avg_long_mip_qavg += _long_mip_qavg;
	  _long_mip_npx = long_mip_cluster->npx();
	  _avg_long_mip_npx += _long_mip_npx;
	  
	  _long_mip_angle = long_mip_cluster->angle();
	  longest_mip_cluster_ptr_v[plane_id] = long_mip_cluster;
	}
	
	if (long_hip_cluster) {
	  _long_hip_width = long_hip_cluster->width();
	  _avg_long_hip_width += _long_hip_width;
	  _long_hip_qsum = long_hip_cluster->qsum();
	  _avg_long_hip_qsum += _long_hip_qsum;
	  _long_hip_qavg = long_hip_cluster->qavg();
	  _avg_long_hip_qavg += _long_hip_qavg;
	  _long_hip_npx = long_hip_cluster->npx();
	  _avg_long_hip_npx += _long_hip_npx;
	  
	  _long_hip_angle = long_hip_cluster->angle();
	  longest_hip_cluster_ptr_v[plane_id] = long_hip_cluster;
	}

	if (num_hips > 0) {
	  _hip_per_plane = 1;
	  was_hip+=1;
	}
	if (num_mips > 0) {
	  was_mip+=1;	  
	}
	
	_hip_plane_tree->Fill();
	ClearHIPCluster();
      } // end plane wise hipcluster
      
      _avg_long_mip_width /= was_mip;
      _avg_long_hip_width /= was_hip;
      
      _avg_long_mip_qsum /= was_mip;
      _avg_long_hip_qsum /= was_hip;
      
      _avg_long_mip_qavg /= was_mip;
      _avg_long_hip_qavg /= was_hip;
      
      _avg_long_mip_npx /= was_mip;
      _avg_long_hip_npx /= was_hip;

      _avg_long_mip_length /= was_mip;
      _avg_long_hip_length /= was_hip;
      
      _hip_event_tree->Fill();
      
    } // end event wise hipcluster
    
    //
    //
    //
    //DefectClusterAlgoStuff
    auto defectcluster_id = dm.ID(_defectcluster_name);
    if (defectcluster_id != larocv::kINVALID_ALGO_ID ) {
      
      const auto& img_v = _mgr_ptr->InputImages();
      std::vector<cv::Mat> thresh_img_v;
      thresh_img_v.resize(3);
      for(uint img_id=0; img_id < 3; ++img_id) {
	const auto& img  = img_v[img_id];
	auto& thresh_img = thresh_img_v[img_id];
	cv::threshold(img, thresh_img, 10,255, CV_THRESH_BINARY);
      }
      
      const auto defectcluster_data = (larocv::data::DefectClusterData*)dm.Data( dm.ID(_defectcluster_name) );

      for(uint plane_id=0; plane_id < 3;  ++plane_id) {

	_defect_matched_x  = kINVALID_FLOAT;
	_defect_matched_y  = kINVALID_FLOAT;
	_defect_matched_d  = kINVALID_FLOAT;
	_defect_matched_id = kINVALID_INT;
	_defect_has_match = 0;
	  
	const auto& img        = img_v[plane_id];
	const auto& thresh_img = thresh_img_v[plane_id];
	  
	const auto& longest_mip = longest_mip_cluster_ptr_v[plane_id];
	const auto& longest_hip = longest_hip_cluster_ptr_v[plane_id];
	geo2d::Vector<float> matched_defect(kINVALID_FLOAT,kINVALID_FLOAT);
	
	_defect_plane_id   = plane_id;
	
	std::vector<cv::Point_<int> >  points;
	cv::findNonZero(thresh_img, points);
	
	_defect_n_defects=0;
	
	auto& clustercompound_data = defectcluster_data->_raw_cluster_vv[plane_id];

	int num_cluster    = clustercompound_data.num_clusters();
	int defect_id      = 0;
	float ddist = kINVALID_FLOAT;
	
	for (uint cluster_id = 0; cluster_id < num_cluster; cluster_id++){
	  
	  const auto& cluster_data = clustercompound_data.get_cluster()[cluster_id];
	  const auto& defect_data = cluster_data.get_defects();
	  const auto& atom_data   = cluster_data.get_atoms();
	  
	  int num_defects   = defect_data.size();
	  
	  _defect_n_defects+=num_defects;
	  
	  geo2d::Vector<float> start, end; 

	  for (uint cluster_defect_id = 0; cluster_defect_id < num_defects; cluster_defect_id++){

	    const auto& defect        = defect_data[cluster_defect_id];
	    const auto& ass_atom_id_v = defect.associated_atoms();
	    
	    start = defect._pt_start;
	    end   = defect._pt_end;
	    
	    _defect_n_atomics      = ass_atom_id_v.size();
	    _defect_dist           = defect._dist;
	    _defect_dist_start_end = sqrt(pow(start.x-end.x,2)+pow(start.y-end.y,2));
	    _defect_id             = defect_id;

	    for(auto atom_idx : ass_atom_id_v) {

	      const auto& ctor = atom_data.at(atom_idx)._ctor;
	      
	      uint npts = 0;
	      uint qsum = 0;
      
	      for(const auto& pt : points) { 
		if( cv::pointPolygonTest(ctor,pt,false) >= 0 ) {
		  npts+=1;
		  qsum += (uchar) img.at<uchar>(pt.y,pt.x);
		}
	      }
	      
	      auto rect = cv::minAreaRect(ctor);
	      auto height = rect.size.height;
	      auto width  = rect.size.width;

	      auto length = height > width ? height : width;
	      
	      _defect_atomic_len_v.push_back(length);
	      _defect_atomic_qsum_v.push_back(qsum);
	      _defect_atomic_npts_v.push_back(npts);
	      _defect_atomic_qavg_v.push_back( (float) qsum / (float) npts);
	      
	    }
	    
	    defect_id++;

	    auto dist = dist_defect_hip_mip(longest_mip,longest_hip,defect._pt_defect);
	    // std::cout << "Observed " << defect._pt_defect << " dist: " << dist << std::endl;
	    if (dist < ddist) {
	      // std::cout << "\t" << "~smaller~" << std::endl;
	      ddist = dist;
	      matched_defect = defect._pt_defect;
	      _defect_matched_id = defect_id;
	      _defect_has_match = 1;
	    }
	    
	    _defect_defect_tree->Fill();
	    ClearDefect();
	  } // end loop over defect per cluster
	} // end loop over cluster

	
	//matched defect in hand
	// std::cout << "Matched " << matched_defect << " avg dist: " << ddist << std::endl;

	_defect_matched_x  = matched_defect.x;
	_defect_matched_y  = matched_defect.y;
	_defect_matched_d  = ddist;
	
	_defect_plane_tree->Fill();
	
      } // end loop over 3 planes

      _defect_event_tree->Fill();
    } // end conditional defectcluster algo name
    //
    //
    //
    
    if ( !_refine2dvertex_name.empty() &&
	 !_vertexcluster_name.empty() &&
	 !_linearvtxfilter_name.empty() ) {
      
      /// Refine2D data
      const auto refine2d_data = (larocv::data::Refine2DVertexData*)dm.Data( dm.ID(_refine2dvertex_name) );
      
      /// VertexCluster data
      const auto vtxtrkcluster_data = (larocv::data::VertexClusterArray*)dm.Data( dm.ID(_vertexcluster_name) );
      
      /// LinearVtxFilter data
      auto linearvf_data = (larocv::data::LinearVtxFilterData*) dm.Data( dm.ID(_linearvtxfilter_name) );
      
      //careful: this is nonconst
      auto& circle_setting_array_v = linearvf_data->_circle_setting_array_v;
      
      auto& vtx_cluster_v=  vtxtrkcluster_data->_vtx_cluster_v;
      
      _n_vtx3d = (uint) vtx_cluster_v.size();
      
      for(uint vtx_id=0;vtx_id<_n_vtx3d;++vtx_id) {
	
	// clear vertex
	ClearVertex();
	
	// set the vertex ID
	_vtx3d_id=vtx_id;
	
	// get this VertexTrackCluster
	const auto& vtx_cluster = vtx_cluster_v[vtx_id];
	
	// set the vertex type
	_vtx3d_type = (uint) refine2d_data->get_type(vtx_id);
	
	// get this 3D vertex
	const auto& vtx3d = vtx_cluster.get_vertex();
	
	//get this circle's setting
	auto& csarray = circle_setting_array_v[vtx_id];
	
	_vtx3d_x = vtx3d.x;
	_vtx3d_y = vtx3d.y;
	_vtx3d_z = vtx3d.z;
      
	_num_planes = (uint) vtx3d.num_planes;
      
	_sum_pixel_frac  = 0.0;
	_prod_pixel_frac = 1.0;
      
	for(uint plane_id=0; plane_id<3;  ++plane_id) {
	
	  _plane_id=plane_id;
	
	  const auto& circle_vtx   = vtx_cluster.get_circle_vertex(plane_id);
	  const auto& circle_vtx_c = circle_vtx.center;
	  
	  auto& circle_x  = _circle_x_v [plane_id];
	  auto& circle_y  = _circle_y_v [plane_id];
	  auto& circle_xs = _circle_xs_v[plane_id];
	  
	  circle_x = circle_vtx_c.x;
	  circle_y = circle_vtx_c.y;
	  
	  circle_xs = (uint) circle_vtx.xs_v.size();
	  
	  auto& num_clusters   = _num_clusters_v[plane_id];
	  auto& num_pixels     = _num_pixels_v[plane_id];
	  auto& num_pixel_frac = _num_pixel_frac_v[plane_id];
	  
	  num_clusters   = vtx_cluster.num_clusters(plane_id);
	  num_pixels     = vtx_cluster.num_pixels(plane_id);
	  num_pixel_frac = vtx_cluster.num_pixel_fraction(plane_id);
	  
	  _sum_pixel_frac  += num_pixel_frac;
	  _prod_pixel_frac *= num_pixel_frac; 
	  
	  auto& vtx2d_x = _vtx2d_x_v[plane_id];
	  auto& vtx2d_y = _vtx2d_y_v[plane_id];
	  
	  vtx2d_x = vtx3d.vtx2d_v[plane_id].pt.x;
	  vtx2d_y = vtx3d.vtx2d_v[plane_id].pt.y;
	  
	  const auto& csetting = csarray.get_circle_setting(plane_id);
	  
	  auto& circle_vtx_r     = _circle_vtx_r_v[plane_id];
	  auto& circle_vtx_angle = _circle_vtx_angle_v[plane_id];
	  
	  circle_vtx_r     = csetting._local_r;
	  circle_vtx_angle = csetting._angle;

	  /// dQdX profiler
	  if ( !_dqdxprofiler_name.empty() ) {
	  
	    auto dqdxprofiler_data = (larocv::data::dQdXProfilerData*)dm.Data( dm.ID(_dqdxprofiler_name) );
	    
	    //get the dqdx particle array for this vertex id
	    const auto& pardqdxarr = dqdxprofiler_data->get_vertex_cluster(vtx_id);
	    
	    //list of particles on this plane
	    const auto& pardqdx_v = pardqdxarr.get_cluster(plane_id);
	    
	    //particle list from vertextrackcluster
	    const auto& parcluster_v = vtx_cluster.get_clusters(plane_id);
	    
	    ClearParticle();
	    
	    _n_pars = pardqdx_v.size();
	    _num_atoms_v.resize(_n_pars);
	    _start_x_v.resize(_n_pars);
	    _start_y_v.resize(_n_pars);
	    _end_x_v.resize(_n_pars);
	    _end_y_v.resize(_n_pars);
	    _start_end_length_v.resize(_n_pars);
	    _atom_sum_length_v.resize(_n_pars);
	    _first_atom_cos_v.resize(_n_pars);
	    _qsum_v.resize(_n_pars);
	    _npix_v.resize(_n_pars);
	    _dqdx_vv.resize(_n_pars);
	    _dqdx_start_idx_vv.resize(_n_pars);
	    
	    for(uint pidx=0; pidx < _n_pars; ++pidx) {
	      
	      auto& num_atoms        = _num_atoms_v[pidx];
	      auto& start_x          = _start_x_v[pidx];
	      auto& start_y          = _start_y_v[pidx];
	      auto& end_x            = _end_x_v[pidx];
	      auto& end_y            = _end_y_v[pidx];
	      auto& start_end_length = _start_end_length_v[pidx];
	      auto& atom_sum_length  = _atom_sum_length_v[pidx];
	      auto& first_atom_cos   = _first_atom_cos_v[pidx];
	      auto& qsum             = _qsum_v[pidx];
	      auto& npix             = _npix_v[pidx];
	      
	      //this is a special from VertexTrackCluster
	      const auto& parcluster = parcluster_v[pidx];
	      
	      npix = parcluster._num_pixel;
	      qsum = parcluster._qsum;
	      
	      const auto& pardqdx = pardqdx_v[pidx];
	      
	      num_atoms = pardqdx.num_atoms();
	      start_x = pardqdx.start_pt().x;
	      start_y = pardqdx.start_pt().y;
	      end_x  = pardqdx.end_pt().x;
	      end_y  = pardqdx.end_pt().y;
	      
	      start_end_length = geo2d::length(pardqdx.end_pt() - pardqdx.start_pt());
	      atom_sum_length=0.0;
	      
	      //loop over ordered atomics and calcluate the start end length 1-by-1, sum them
	      for(auto aid : pardqdx.atom_id_array())
		atom_sum_length += geo2d::length(pardqdx.atom_end_pt(aid) - pardqdx.atom_start_pt(aid));
	      
	      //use the first atomic to estimate the direction, get the first atomic end point location
	      //calculate direction from 2D vertex
	      const auto& first_atom_end = pardqdx.atom_end_pt(0);
	      
	      auto dir=first_atom_end-geo2d::Vector<float>(vtx2d_x,vtx2d_y);
	      double cosangle = dir.x / sqrt(dir.x*dir.x + dir.y*dir.y);
	      
	      first_atom_cos = cosangle;
	      
	      auto& dqdx_v = _dqdx_vv[pidx];
	      dqdx_v = pardqdx.dqdx(); // copy it.
	      
	      auto& dqdx_start_idx_v = _dqdx_start_idx_vv[pidx];
	      dqdx_start_idx_v.resize(num_atoms);
	      
	      for(uint atom_id=0;atom_id<num_atoms;++atom_id)
		dqdx_start_idx_v[atom_id] = (uint) pardqdx.atom_start_index(atom_id);
	      
	    } //end per particle
	    _particle_tree->Fill();
	  } // end dqdx block
	} // end plane
	_vtx3d_tree->Fill();
      } // end loop over vtx
      _vtx3d_event_tree->Fill();
    } // end vtx block
      
    if (!_lineartrackcluster_name.empty()) {
	
      /// LinearTrackCluster
      auto lineartrackcluster_data = (larocv::data::LinearTrackArray*) dm.Data( dm.ID(_lineartrackcluster_name) );
      const auto& track_clusters = lineartrackcluster_data->get_clusters();

      _n_trackclusters = (uint) track_clusters.size();

      for( uint trk_cluster_idx=0; trk_cluster_idx < track_clusters.size(); trk_cluster_idx++) {

	ClearTracks();
      
	const auto& trk_cluster = lineartrackcluster_data->get_cluster(trk_cluster_idx);
      
	for( uint plane_id=0; plane_id<3; ++plane_id) {

	  auto& edge2D_1_x = _edge2D_1_x_v[plane_id];
	  auto& edge2D_1_y = _edge2D_1_y_v[plane_id];
	  auto& edge2D_2_x = _edge2D_2_x_v[plane_id];
	  auto& edge2D_2_y = _edge2D_2_y_v[plane_id];
	
	  const auto& trk_linear2d = trk_cluster.get_cluster(plane_id);

	  const auto& edge1 = trk_linear2d.edge1;
	  const auto& edge2 = trk_linear2d.edge2;
	
	  edge2D_1_x = edge1.x;
	  edge2D_1_y = edge1.y;
	  edge2D_2_x = edge2.x;
	  edge2D_2_y = edge2.y;
	
	}
      
	_track_tree->Fill();
      }

      
      /// VertexSingleShower
      auto vertexsingleshower_data = (larocv::data::SingleShowerArray*) dm.Data( dm.ID(_vertexsingleshower_name) );
      const auto& shower_clusters = vertexsingleshower_data->get_showers();
      
      _n_showerclusters = (uint) shower_clusters.size();

      for( uint shr_cluster_idx=0; shr_cluster_idx < shower_clusters.size(); shr_cluster_idx++) {

	ClearShowers();
      
	const auto& shr_cluster = shower_clusters[shr_cluster_idx];

	_shower_id       = shr_cluster.id();
	_shower_ass_id   = shr_cluster.ass_id();
	_shower_ass_type = shr_cluster.ass_type();

	const auto& shower_vtx3d = shr_cluster.get_vertex();
	_shower_vtx3D_x = shower_vtx3d.x;
	_shower_vtx3D_y = shower_vtx3d.y;
	_shower_vtx3D_z = shower_vtx3d.z;

	for( uint plane_id=0; plane_id<3; ++plane_id) {

	  auto& start2D_x = _start2D_x_v[plane_id];
	  auto& start2D_y = _start2D_y_v[plane_id];
	  auto& dir2D_x   = _dir2D_x_v[plane_id];
	  auto& dir2D_y   = _dir2D_y_v[plane_id];

	  larocv::data::ShowerCluster shr_cluster2d;
	  try {
	    shr_cluster2d = shr_cluster.get_cluster(plane_id);
	  }
	  catch(...) {
	    continue;
	  }

	  const auto& start = shr_cluster2d.start;
	  const auto& dir   = shr_cluster2d.dir;

	  start2D_x = start.x;
	  start2D_y = start.y;

	  dir2D_x   = dir.x;
	  dir2D_y   = dir.y;
	
	}
      
	_shower_tree->Fill();
      }
    }
    
    _event_tree->Fill();
    
    return true;
  }

  void LArbysImageAna::finalize()
  {
    _event_tree->Write();
    _hip_event_tree->Write();
    _hip_plane_tree->Write();
    _defect_event_tree->Write();
    _defect_plane_tree->Write();
    _defect_defect_tree->Write();
    _vtx3d_event_tree->Write();
    _vtx3d_tree->Write();
    _particle_tree->Write();
    _track_tree->Write();
    _shower_tree->Write();
  }

}
#endif

