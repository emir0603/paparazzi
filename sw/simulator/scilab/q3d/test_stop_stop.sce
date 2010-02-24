clear()
exec('q3d_utils.sci');

exec('q3d_polynomials.sci');
exec('q3d_sbb.sci');
exec('q3d_fo_traj_misc.sci');

exec('q3d_diff_flatness.sci');
exec('q3d_fdm.sci');
exec('q3d_display.sci');
exec('q3d_povray.sci');


t0 = 0;
t1 = 10.;
dt = 1/512;
time = t0:dt:t1;

if (0)
  // polynomials
  b0 = [0 0 0 0 0; 0 0 0 0 0];
  b1 = [2 0 0 0 0; 0 0 0 0 0];
  [coefs] = poly_get_coef_from_bound(time, b0, b1);
  [fo_traj] = poly_gen_traj(time, coefs);
end
if (0)
// differential equation
  dyn = [rad_of_deg(500) 0.7; rad_of_deg(500) 0.7];
  max_speed = [5 2.5];
  max_accel = [ 9.81*tan(rad_of_deg(29.983325)) 0.5*9.81];
  b0 = [-5 0];
  b1 = [ 5 -2];
  [fo_traj] = sbb_gen_traj(time, dyn, max_speed, max_accel, b0, b1);
  printf('xfinal %f, zfinal:%f\n',fo_traj(1,1,$), fo_traj(2,1,$));
end
if (1)
 [fo_traj] = fo_traj_circle(time, [0 0], 2, rad_of_deg(45));
end
 
 
diff_flat_cmd = zeros(2,length(time));
diff_flat_ref = zeros(FDM_SSIZE, length(time));
for i=1:length(time)
  diff_flat_cmd(:,i) = df_input_of_fo(fo_traj(:,:,i));
  diff_flat_ref(:,i) = df_state_of_fo(fo_traj(:,:,i));
end

fdm_init(time, df_state_of_fo(fo_traj(:,:,1))); 
for i=2:length(time)
  u1 = diff_flat_cmd(1,i-1);
  u2 = diff_flat_cmd(2,i-1);
  m1 = 0.5*(u1+u2);
  m2 = 0.5*(u1-u2);
  fdm_run(i, [m1 m2]')
end

set("current_figure",0);
clf();
f=get("current_figure");
f.figure_name="Flat Outputs Trajectory";
display_fo(time, fo_traj);

set("current_figure",1);
clf();
f=get("current_figure");
f.figure_name="Commands";
display_commands(time, diff_flat_cmd);

set("current_figure",2);
clf();
f=get("current_figure");
f.figure_name="Reference";
display_fo_ref(time, diff_flat_ref);

set("current_figure",3);
clf();
f=get("current_figure");
f.figure_name="FDM";
display_fdm();


povray_draw(time, diff_flat_ref);