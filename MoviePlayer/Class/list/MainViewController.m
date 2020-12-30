//
//  MainViewController.m
//  kxmovie
//
//  Created by Kolyvan on 18.10.12.
//  Copyright (c) 2012 Konstantin Boukreev . All rights reserved.
//
//  https://github.com/kolyvan/kxmovie
//  this file is part of KxMovie
//  KxMovie is licenced under the LGPL v3, see lgpl-3.0.txt

#import "MainViewController.h"
#import "ViewController.h"

@interface MainViewController () {
    NSArray *_localMovies;
    NSArray *_remoteMovies;
}
@property (strong, nonatomic) UITableView *tableView;
@end

@implementation MainViewController

- (id)init
{
    self = [super init];
    if (self) {
        self.title = @"Main";
    }
    return self;
}

- (void)renderList:(id)resp{
    if (![resp isKindOfClass:NSDictionary.class]) {
        return;
    }
    if ([resp[@"lives"] isKindOfClass:NSArray.class]) {
        _remoteMovies = resp[@"lives"];
    }
    dispatch_async(dispatch_get_main_queue(), ^{ [self.tableView reloadData]; });
}

- (void)loadView
{
    self.view = [[UIView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.tableView = [[UITableView alloc] initWithFrame:self.view.bounds style:UITableViewStylePlain];
    self.tableView.backgroundColor = [UIColor whiteColor];
    //self.tableView.backgroundView = [[UIImageView alloc] initWithImage:image];
    self.tableView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleBottomMargin;
    self.tableView.delegate = self;
    self.tableView.dataSource = self;
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleSingleLine;
    
    [self.view addSubview:self.tableView];
}

- (void)viewDidLoad
{
    [super viewDidLoad];

//#ifdef DEBUG_AUTOPLAY
//    [self performSelector:@selector(launchDebugTest) withObject:nil afterDelay:0.5];
//#endif
    NSString *urlString = @"http://service.inke.com/api/live/simpleall?&gender=1&gps_info=116.346766%2C40.090413&loc_info=CN%2C%E5%8C%97%E4%BA%AC%E5%B8%82%2C%E5%8C%97%E4%BA%AC%E5%B8%82&is_new_user=1&lc=0000000000000053&cc=TG0001&cv=IK4.0.30_Iphone&proto=7&idfa=D7D0D5A2-3073-4A74-A726-98BE8B4E8F38&idfv=58A18E13-A21D-456D-B6D8-7499948B379D&devi=54b68af1895085419f7f8978d95d95257dd44f93&osversion=ios_10.300000&ua=iPhone6_2&imei=&imsi=&uid=450515766&sid=20XNNoa5VwMozGALfmi2xN1YCfLWvEq7aJuTHTQLu8bT88i1aNbi0&conn=wifi&mtid=391bb3520c38e0444ba0b3975f4bb1aa&mtxid=f0b42913a33c&logid=162,210&s_sg=89b4fd485d7c5ac30dc0dbf6042a06a9&s_sc=100&s_st=1493023925";
    NSURLRequest *req = [NSURLRequest requestWithURL:[NSURL URLWithString:urlString]];
    [[NSURLSession.sharedSession dataTaskWithRequest:req completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
            
        id json = nil;
        if (data) {
            json = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:nil];
        }
        
        if (json) {
            [self renderList:json];
        }else{
            NSLog(@"error\n%@", error);
        }
            
    }] resume];
    
}

- (void)launchDebugTest
{
    [self tableView:self.tableView didSelectRowAtIndexPath:[NSIndexPath indexPathForRow:4
                                                                              inSection:1]];
}

- (void) reloadMovies
{
    NSMutableArray *ma = [NSMutableArray array];
    NSFileManager *fm = [[NSFileManager alloc] init];
    NSString *folder = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                                            NSUserDomainMask,
                                                            YES) lastObject];
    NSArray *contents = [fm contentsOfDirectoryAtPath:folder error:nil];
    
    for (NSString *filename in contents) {
        
        if (filename.length > 0 &&
            [filename characterAtIndex:0] != '.') {
            
            NSString *path = [folder stringByAppendingPathComponent:filename];
            NSDictionary *attr = [fm attributesOfItemAtPath:path error:nil];
            if (attr) {
                id fileType = [attr valueForKey:NSFileType];
                if ([fileType isEqual: NSFileTypeRegular] ||
                    [fileType isEqual: NSFileTypeSymbolicLink]) {
                    
                    NSString *ext = path.pathExtension.lowercaseString;
                    
                    if ([ext isEqualToString:@"mp3"] ||
                        [ext isEqualToString:@"caff"]||
                        [ext isEqualToString:@"aiff"]||
                        [ext isEqualToString:@"ogg"] ||
                        [ext isEqualToString:@"wma"] ||
                        [ext isEqualToString:@"m4a"] ||
                        [ext isEqualToString:@"m4v"] ||
                        [ext isEqualToString:@"wmv"] ||
                        [ext isEqualToString:@"3gp"] ||
                        [ext isEqualToString:@"mp4"] ||
                        [ext isEqualToString:@"mov"] ||
                        [ext isEqualToString:@"avi"] ||
                        [ext isEqualToString:@"mkv"] ||
                        [ext isEqualToString:@"mpeg"]||
                        [ext isEqualToString:@"mpg"] ||
                        [ext isEqualToString:@"flv"] ||
                        [ext isEqualToString:@"vob"]) {
                        
                        [ma addObject:path];
                    }
                }
            }
        }
    }

    // Add all the movies present in the app bundle.
    NSBundle *bundle = [NSBundle mainBundle];
    [ma addObjectsFromArray:[bundle pathsForResourcesOfType:@"mp4" inDirectory:@"SampleMovies"]];
    [ma addObjectsFromArray:[bundle pathsForResourcesOfType:@"mov" inDirectory:@"SampleMovies"]];
    [ma addObjectsFromArray:[bundle pathsForResourcesOfType:@"m4v" inDirectory:@"SampleMovies"]];
    [ma addObjectsFromArray:[bundle pathsForResourcesOfType:@"wav" inDirectory:@"SampleMovies"]];

    [ma sortedArrayUsingSelector:@selector(compare:)];
    
    _localMovies = [ma copy];
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 2;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    switch (section) {
        case 0:     return @"Remote";
        case 1:     return @"Local";
    }
    return @"";
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    switch (section) {
        case 0:     return _remoteMovies.count;
        case 1:     return _localMovies.count;
    }
    return 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *cellIdentifier = @"Cell";
    UITableViewCell *cell = [self.tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                      reuseIdentifier:cellIdentifier];
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    }
    
    if (indexPath.section == 0) {
        id remote = _remoteMovies[indexPath.row];
        cell.textLabel.text = [remote[@"stream_addr"] lastPathComponent];
    } else {
        cell.textLabel.text = [_localMovies[indexPath.row] lastPathComponent];
    }
    return cell;
}

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [tableView deselectRowAtIndexPath:indexPath animated:NO];
    
    NSString *path;
    if (indexPath.section == 0) {
        
        if (indexPath.row >= _remoteMovies.count) return;
        id remote = _remoteMovies[indexPath.row];
        path = remote[@"stream_addr"];
        
    } else {

        if (indexPath.row >= _localMovies.count) return;
        path = _localMovies[indexPath.row];
    }
    
    ViewController *vc = [ViewController new];
    vc.liveAddr = path;
    [self.navigationController pushViewController:vc animated:YES];
}

@end
