
#include <iostream>
#include <pcl/ModelCoefficients.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/filters/extract_indices.h>
 
void viewerOneOff(pcl::visualization::PCLVisualizer& viewer)
{
    viewer.setBackgroundColor(0,0,1);
}
 
int
main (int argc, char** argv)
{
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PCDReader reader;
         // read into the point cloud file PCD
    //reader.read(argv[1],*cloud);
 	
  if (pcl::io::loadPCDFile<pcl::PointXYZ> (argv[1], *cloud) == -1) //* load the file
  {
   PCL_ERROR ("Couldn't read file test_pcd.pcd \n");
    return (-1);
  }

    std::cerr << "Point cloud data: " << cloud->points.size () << " points" << std::endl;
    
    pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
    pcl::PointIndices::Ptr inliers (new pcl::PointIndices);
    // Create the segmentation object
    pcl::SACSegmentation<pcl::PointXYZ> seg;
    // Optional
    seg.setOptimizeCoefficients (true);
    // Mandatory
    seg.setModelType (pcl::SACMODEL_PLANE);
    seg.setMethodType (pcl::SAC_RANSAC);
         // distance threshold in m
    seg.setDistanceThreshold (0.15);
    seg.setInputCloud (cloud);
    seg.segment (*inliers, *coefficients);
    if (inliers->indices.size () == 0)
    {
        PCL_ERROR ("Could not estimate a planar model for the given dataset.");
        return (-1);
    }
 
 
         // extract ground
    pcl::ExtractIndices<pcl::PointXYZ> extract;
    extract.setInputCloud (cloud);
    extract.setIndices (inliers);
    extract.filter (*cloud_filtered);
 
    std::cerr << "Ground cloud after filtering: " << std::endl;
    std::cerr << *cloud_filtered << std::endl;
 
    pcl::PCDWriter writer;
    writer.write<pcl::PointXYZ> ("3dpoints_ground.pcd", *cloud_filtered, false);
 
         // Extract addition to the ground
    extract.setNegative (true);
    extract.filter (*cloud_filtered);
 
    std::cerr << "Object cloud after filtering: " << std::endl;
    std::cerr << *cloud_filtered << std::endl;
 
    writer.write<pcl::PointXYZ> ("3dpoints_object.pcd", *cloud_filtered, false);
 
         // point cloud visualization
    pcl::visualization::CloudViewer viewer("Filtered");
    viewer.showCloud(cloud_filtered);
    viewer.runOnVisualizationThreadOnce(viewerOneOff);
    while(!viewer.wasStopped()){
    
    }
    return (0);
}
